#include "ml/classifier.hpp"
#include "ml/tokenizer.hpp"
#include <algorithm>

namespace securelens {

Classifier::Classifier(const Model& model) : model_(model) {}

SentimentResult Classifier::classify(const std::string& text) const {
    Tokenizer tokenizer;
    auto tokens = tokenizer.tokenize(text);
    auto vec = model_.tfidf().transform(tokens);
    auto probs = model_.logisticRegression().predict_proba(vec);

    if (probs.empty()) {
        return {"neutral", 0.0};
    }

    auto max_it = std::max_element(probs.begin(), probs.end());
    int idx = static_cast<int>(max_it - probs.begin());
    double confidence = *max_it;

    std::string sentiment = "neutral";
    if (idx >= 0 && idx < static_cast<int>(model_.classes().size())) {
        sentiment = model_.classes()[idx];
    }

    return {sentiment, confidence};
}

}
