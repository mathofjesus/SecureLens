#include "api/security_routes.hpp"
#include "api/sentiment_routes.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>

using namespace securelens;

static const int PORT = 8080;
static const int BUFFER_SIZE = 8192;

static std::string extractBody(const std::string& request) {
    auto pos = request.find("\r\n\r\n");
    if (pos == std::string::npos) return "";
    return request.substr(pos + 4);
}

static std::string extractPath(const std::string& request) {
    auto start = request.find(' ');
    if (start == std::string::npos) return "";
    auto end = request.find(' ', start + 1);
    if (end == std::string::npos) return "";
    return request.substr(start + 1, end - start - 1);
}

static std::string extractMethod(const std::string& request) {
    auto end = request.find(' ');
    if (end == std::string::npos) return "";
    return request.substr(0, end);
}

static void sendResponse(int client_fd, int status, const std::string& body) {
    std::string status_text = (status == 200) ? "OK" : "Bad Request";
    std::ostringstream response;
    response << "HTTP/1.1 " << status << " " << status_text << "\r\n";
    response << "Content-Type: application/json\r\n";
    response << "Content-Length: " << body.length() << "\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << body;

    std::string resp = response.str();
    send(client_fd, resp.c_str(), resp.length(), 0);
}

static void handleRequest(int client_fd, const std::string& request) {
    std::string method = extractMethod(request);
    std::string path = extractPath(request);
    std::string body = extractBody(request);

    std::string response_body;
    int status = 200;

    if (method == "GET" && path == "/health") {
        response_body = R"({"status":"healthy"})";
    } else if (method == "POST" && path == "/api/v1/security/scan") {
        response_body = handleSecurityScan(body);
    } else if (method == "POST" && path == "/api/v1/sentiment") {
        response_body = handleSentiment(body);
    } else if (method == "POST" && path == "/api/v1/analyze") {
        auto sec_result = handleSecurityScan(body);
        auto sent_result = handleSentiment(body);
        response_body = "{\"security\":" + sec_result + ",\"sentiment\":" + sent_result + "}";
    } else {
        status = 404;
        response_body = R"({"error":"Not found"})";
    }

    sendResponse(client_fd, status, response_body);
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Failed to bind to port " << PORT << "\n";
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 10) < 0) {
        std::cerr << "Failed to listen\n";
        close(server_fd);
        return 1;
    }

    std::cout << "SecureLens API running on http://localhost:" << PORT << "\n";

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

        if (client_fd < 0) continue;

        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_read > 0) {
            std::string request(buffer, static_cast<size_t>(bytes_read));
            handleRequest(client_fd, request);
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}
