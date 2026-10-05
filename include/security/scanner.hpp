#pragma once
#include "security/dependency.hpp"
#include "security/osv_client.hpp"
#include <string>
#include <vector>

namespace securelens {

struct ScanResult {
    std::string repository;
    int dependencies = 0;
    int vulnerabilities = 0;
    std::vector<Vulnerability> findings;
};

class Scanner {
public:
    ScanResult scanDirectory(const std::string& path) const;
    ScanResult scanRepository(const std::string& url) const;
};

}
