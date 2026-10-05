#include "ml/tfidf.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace securelens {

void Tfidf::fit(const std::vector<std::vector<std::string>>& documents) {
    std::unordered_map<std::string, int> doc_freq;
    int n_docs = static_cast<int>(documents.size());

    for (const auto& doc : documents) {
        std::unordered_map<std::string, bool> seen;
        for (const auto& word : doc) {
            if (!seen[word]) {
                doc_freq[word]++;
                seen[word] = true;
            }
        }
    }

    vocab_.clear();
    word_to_idx_.clear();
    idf_.clear();

    for (const auto& [word, freq] : doc_freq) {
        vocab_.push_back(word);
    }

    std::sort(vocab_.begin(), vocab_.end());

    for (int i = 0; i < static_cast<int>(vocab_.size()); ++i) {
        word_to_idx_[vocab_[i]] = i;
    }

    for (const auto& word : vocab_) {
        double idf_val = std::log((1.0 + n_docs) / (1.0 + doc_freq[word])) + 1.0;
        idf_[word] = idf_val;
    }
}

std::vector<double> Tfidf::transform(const std::vector<std::string>& tokens) const {
    std::vector<double> vec(vocab_.size(), 0.0);
    if (tokens.empty()) return vec;

    std::unordered_map<std::string, int> tf;
    for (const auto& word : tokens) {
        tf[word]++;
    }

    for (const auto& [word, count] : tf) {
        auto it = word_to_idx_.find(word);
        auto idf_it = idf_.find(word);
        if (it != word_to_idx_.end() && idf_it != idf_.end()) {
            double tf_val = 1.0 + std::log(static_cast<double>(count));
            vec[it->second] = tf_val * idf_it->second;
        }
    }

    return vec;
}

const std::vector<std::string>& Tfidf::vocabulary() const {
    return vocab_;
}

int Tfidf::vocabSize() const {
    return static_cast<int>(vocab_.size());
}

}
