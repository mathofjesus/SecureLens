#include "security/parsers/package_json_parser.hpp"
#include <nlohmann/json.hpp>

namespace securelens {

using json = nlohmann::json;

std::vector<Dependency> PackageJsonParser::parse(const std::string& content) const {
    std::vector<Dependency> deps;

    try {
        auto j = json::parse(content);

        auto parse_deps = [&](const char* key) {
            if (j.contains(key) && j[key].is_object()) {
                for (auto& [name, version] : j[key].items()) {
                    if (version.is_string()) {
                        deps.push_back({"npm", name, version.get<std::string>(), "", ""});
                    }
                }
            }
        };

        parse_deps("dependencies");
        parse_deps("devDependencies");
    } catch (const json::exception&) {
        return {};
    }

    return deps;
}

}
