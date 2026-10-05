#pragma once
#include "security/dependency.hpp"
#include <string>
#include <vector>

namespace securelens {

struct Vulnerability {
    std::string id;
    std::string cve;
    std::string severity;
    std::string summary;
    std::string fixed_version;
    std::string package;
    std::string installed_version;
};

class OSVClient {
public:
    std::vector<Vulnerability> query(const std::string& ecosystem,
                                      const std::string& name,
                                      const std::string& version) const;
};

}
