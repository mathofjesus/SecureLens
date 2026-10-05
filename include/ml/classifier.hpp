#pragma once
#include "ml/model.hpp"
#include <string>

namespace securelens {

struct SentimentResult {
    std::string sentiment;
    double confidence;
};

class Classifier {
public:
    explicit Classifier(const Model& model);
    SentimentResult classify(const std::string& text) const;

private:
    const Model& model_;
};

}
