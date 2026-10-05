#pragma once
#include <string>
#include <unordered_map>
#include <vector>

namespace securelens {

class Tfidf {
public:
    void fit(const std::vector<std::vector<std::string>>& documents);
    std::vector<double> transform(const std::vector<std::string>& tokens) const;
    const std::vector<std::string>& vocabulary() const;
    int vocabSize() const;

private:
    std::vector<std::string> vocab_;
    std::unordered_map<std::string, int> word_to_idx_;
    std::unordered_map<std::string, double> idf_;
};

}
