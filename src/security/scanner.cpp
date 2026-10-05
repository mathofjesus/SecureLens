#include "security/scanner.hpp"
#include "security/parsers/requirements_parser.hpp"
#include "security/parsers/package_json_parser.hpp"
#include "security/parsers/pom_parser.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>

namespace securelens {

namespace fs = std::filesystem;

static std::string readFileSafe(const fs::path& path) {
    std::error_code ec;
    auto size = fs::file_size(path, ec);
    if (ec || size > 1024 * 1024) return "";

    std::ifstream file(path);
    if (!file) return "";

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static std::vector<Dependency> parseFile(const fs::path& path) {
    std::string content = readFileSafe(path);
    if (content.empty()) return {};

    std::string filename = path.filename().string();

    if (filename == "requirements.txt") {
        RequirementsParser parser;
        return parser.parse(content);
    } else if (filename == "package.json") {
        PackageJsonParser parser;
        return parser.parse(content);
    } else if (filename == "pom.xml") {
        PomParser parser;
        return parser.parse(content);
    }

    return {};
}

ScanResult Scanner::scanDirectory(const std::string& path) const {
    ScanResult result;
    result.repository = path;

    std::error_code ec;
    if (!fs::exists(path, ec) || !fs::is_directory(path, ec)) {
        return result;
    }

    OSVClient osv;

    for (const auto& entry : fs::recursive_directory_iterator(path)) {
        if (!entry.is_regular_file()) continue;

        auto deps = parseFile(entry.path());
        for (const auto& dep : deps) {
            result.dependencies++;
            auto vulns = osv.query(dep.ecosystem, dep.name, dep.version);
            for (const auto& v : vulns) {
                result.findings.push_back(v);
                result.vulnerabilities++;
            }
        }
    }

    return result;
}

ScanResult Scanner::scanRepository(const std::string& url) const {
    ScanResult result;
    result.repository = url;

    std::string tmpdir = "/tmp/securelens_scan_" + std::to_string(std::hash<std::string>{}(url));
    std::error_code ec;
    fs::create_directories(tmpdir, ec);

    std::string clone_cmd = "git clone --depth 1 \"" + url + "\" \"" + tmpdir + "\" 2>/dev/null";
    int ret = std::system(clone_cmd.c_str());

    if (ret != 0) {
        fs::remove_all(tmpdir, ec);
        return result;
    }

    result = scanDirectory(tmpdir);
    result.repository = url;

    fs::remove_all(tmpdir, ec);
    return result;
}

}
