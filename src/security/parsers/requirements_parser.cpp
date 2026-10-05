#include "security/parsers/requirements_parser.hpp"
#include <sstream>
#include <algorithm>

namespace securelens {

std::vector<Dependency> RequirementsParser::parse(const std::string& content) const {
    std::vector<Dependency> deps;
    std::istringstream stream(content);
    std::string line;

    while (std::getline(stream, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty() || line[0] == '#') continue;

        auto eq_pos = line.find("==");
        if (eq_pos == std::string::npos) continue;

        std::string name = line.substr(0, eq_pos);
        std::string version = line.substr(eq_pos + 2);

        name.erase(0, name.find_first_not_of(" \t"));
        name.erase(name.find_last_not_of(" \t") + 1);
        version.erase(0, version.find_first_not_of(" \t"));
        version.erase(version.find_last_not_of(" \t") + 1);

        if (name.empty() || version.empty()) continue;

        deps.push_back({"pypi", name, version, "", ""});
    }

    return deps;
}

}
