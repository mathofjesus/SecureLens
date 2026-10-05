#include "security/parsers/pom_parser.hpp"
#include <tinyxml2.h>

namespace securelens {

std::vector<Dependency> PomParser::parse(const std::string& content) const {
    std::vector<Dependency> deps;

    tinyxml2::XMLDocument doc;
    if (doc.Parse(content.c_str()) != tinyxml2::XML_SUCCESS) {
        return {};
    }

    auto* project = doc.FirstChildElement("project");
    if (!project) return {};

    auto* deps_elem = project->FirstChildElement("dependencies");
    if (!deps_elem) return {};

    for (auto* dep = deps_elem->FirstChildElement("dependency"); dep; dep = dep->NextSiblingElement("dependency")) {
        std::string group, artifact, version;

        if (auto* g = dep->FirstChildElement("groupId")) {
            group = g->GetText() ? g->GetText() : "";
        }
        if (auto* a = dep->FirstChildElement("artifactId")) {
            artifact = a->GetText() ? a->GetText() : "";
        }
        if (auto* v = dep->FirstChildElement("version")) {
            version = v->GetText() ? v->GetText() : "";
        }

        if (!artifact.empty()) {
            deps.push_back({"maven", artifact, version, group, artifact});
        }
    }

    return deps;
}

}
