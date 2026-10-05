#pragma once
#include <string>
#include <vector>

namespace securelens {

class Tokenizer {
public:
    std::vector<std::string> tokenize(const std::string& text) const;
    std::string normalize(const std::string& text) const;
};

}
