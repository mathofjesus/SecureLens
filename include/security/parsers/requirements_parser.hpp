#pragma once
#include "security/dependency.hpp"
#include <string>
#include <vector>

namespace securelens {

class RequirementsParser {
public:
    std::vector<Dependency> parse(const std::string& content) const;
};

}
