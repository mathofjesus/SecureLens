#pragma once
#include <string>
#include <vector>

namespace securelens {

struct Dependency {
    std::string ecosystem;
    std::string name;
    std::string version;
    std::string group;
    std::string artifact;
};

}
