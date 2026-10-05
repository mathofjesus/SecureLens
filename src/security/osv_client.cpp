#include "security/osv_client.hpp"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <sstream>

namespace securelens {

using json = nlohmann::json;

static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::vector<Vulnerability> OSVClient::query(const std::string& ecosystem,
                                             const std::string& name,
                                             const std::string& version) const {
    std::vector<Vulnerability> vulns;

    CURL* curl = curl_easy_init();
    if (!curl) return vulns;

    json request_body;
    request_body["package"]["name"] = name;
    request_body["package"]["ecosystem"] = ecosystem;
    request_body["version"] = version;

    std::string request_str = request_body.dump();
    std::string response;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.osv.dev/v1/query");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_str.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);

    CURLcode res = curl_easy_perform(curl);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) return vulns;

    try {
        auto j = json::parse(response);
        if (!j.contains("vulns")) return vulns;

        for (const auto& v : j["vulns"]) {
            Vulnerability vuln;
            vuln.id = v.value("id", "");
            vuln.summary = v.value("summary", "");

            if (v.contains("aliases")) {
                for (const auto& alias : v["aliases"]) {
                    std::string a = alias.get<std::string>();
                    if (a.find("CVE-") == 0) {
                        vuln.cve = a;
                        break;
                    }
                }
            }

            if (v.contains("database_specific") && v["database_specific"].contains("severity")) {
                vuln.severity = v["database_specific"]["severity"].get<std::string>();
            }

            if (v.contains("affected")) {
                for (const auto& aff : v["affected"]) {
                    vuln.package = aff.value("package", json::object()).value("name", "");
                    if (aff.contains("ranges")) {
                        for (const auto& range : aff["ranges"]) {
                            if (range.contains("events")) {
                                for (const auto& event : range["events"]) {
                                    if (event.contains("fixed")) {
                                        vuln.fixed_version = event["fixed"].get<std::string>();
                                    }
                                }
                            }
                        }
                    }
                }
            }

            vuln.installed_version = version;
            vulns.push_back(vuln);
        }
    } catch (const json::exception&) {
        return {};
    }

    return vulns;
}

}
