#include "ml/tokenizer.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <unordered_set>

namespace securelens {

static const std::unordered_set<std::string> STOPWORDS = {
    "the", "a", "an", "is", "are", "was", "were", "be", "been", "being",
    "have", "has", "had", "do", "does", "did", "will", "would", "could",
    "should", "may", "might", "shall", "can", "need", "dare", "ought",
    "used", "to", "of", "in", "for", "on", "with", "at", "by", "from",
    "as", "into", "through", "during", "before", "after", "above", "below",
    "between", "out", "off", "over", "under", "again", "further", "then",
    "once", "here", "there", "when", "where", "why", "how", "all", "both",
    "each", "few", "more", "most", "other", "some", "such", "no", "nor",
    "not", "only", "own", "same", "so", "than", "too", "very", "just",
    "because", "but", "and", "or", "if", "while", "about", "up", "it",
    "this", "that", "these", "those", "i", "me", "my", "we", "our",
    "you", "your", "he", "him", "his", "she", "her", "its", "they", "them"
};

std::string Tokenizer::normalize(const std::string& text) const {
    std::string result;
    result.reserve(text.size());

    for (char c : text) {
        if (std::isalpha(static_cast<unsigned char>(c))) {
            result += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        } else if (std::isspace(static_cast<unsigned char>(c))) {
            result += ' ';
        }
    }

    return result;
}

std::vector<std::string> Tokenizer::tokenize(const std::string& text) const {
    std::vector<std::string> tokens;
    std::string normalized = normalize(text);
    std::istringstream stream(normalized);
    std::string word;

    while (stream >> word) {
        if (word.length() > 2 && STOPWORDS.find(word) == STOPWORDS.end()) {
            tokens.push_back(word);
        }
    }

    return tokens;
}

}
