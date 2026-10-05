#include "api/security_routes.hpp"
#include "security/scanner.hpp"
#include <nlohmann/json.hpp>

namespace securelens {

using json = nlohmann::json;

std::string handleSecurityScan(const std::string& body) {
    try {
        auto j = json::parse(body);

        if (!j.contains("repository") && !j.contains("path")) {
            return "{\"error\":\"Missing 'repository' or 'path' field\"}";
        }

        Scanner scanner;
        ScanResult result;

        if (j.contains("path")) {
            std::string path = j["path"].get<std::string>();
            result = scanner.scanDirectory(path);
        } else {
            std::string repo = j["repository"].get<std::string>();
            result = scanner.scanRepository(repo);
        }

        json response;
        response["repository"] = result.repository;
        response["dependencies"] = result.dependencies;
        response["vulnerabilities"] = result.vulnerabilities;

        json findings = json::array();
        for (const auto& v : result.findings) {
            json f;
            f["id"] = v.id;
            f["cve"] = v.cve;
            f["severity"] = v.severity;
            f["summary"] = v.summary;
            f["fixed_version"] = v.fixed_version;
            f["package"] = v.package;
            f["installed_version"] = v.installed_version;
            findings.push_back(f);
        }
        response["findings"] = findings;

        return response.dump();
    } catch (const json::exception& e) {
        return std::string("{\"error\":\"Invalid JSON: ") + e.what() + "\"}";
    } catch (const std::exception& e) {
        return std::string("{\"error\":\"") + e.what() + "\"}";
    }
}

}
