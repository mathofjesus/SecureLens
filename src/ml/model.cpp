#include "ml/model.hpp"
#include <fstream>
#include <sstream>

namespace securelens {

bool Model::save(const std::string& path) const {
    std::ofstream file(path);
    if (!file) return false;

    file << "CLASSES " << classes_.size() << "\n";
    for (const auto& c : classes_) {
        file << c << "\n";
    }

    file << "VOCAB " << tfidf_.vocabSize() << "\n";
    for (const auto& word : tfidf_.vocabulary()) {
        file << word << "\n";
    }

    const auto& lr = logisticRegression();
    file << "LR " << lr.numClasses() << " " << (lr.weights().empty() ? 0 : lr.weights()[0].size()) << "\n";

    for (const auto& row : lr.weights()) {
        for (size_t i = 0; i < row.size(); ++i) {
            if (i > 0) file << " ";
            file << row[i];
        }
        file << "\n";
    }

    for (size_t i = 0; i < lr.bias().size(); ++i) {
        if (i > 0) file << " ";
        file << lr.bias()[i];
    }
    file << "\n";

    return true;
}

bool Model::load(const std::string& path) {
    std::ifstream file(path);
    if (!file) return false;

    std::string line;
    std::getline(file, line);
    if (line.substr(0, 7) != "CLASSES") return false;

    int n_classes = std::stoi(line.substr(8));
    classes_.clear();
    for (int i = 0; i < n_classes; ++i) {
        std::getline(file, line);
        classes_.push_back(line);
    }

    std::getline(file, line);
    if (line.substr(0, 6) != "VOCAB ") return false;

    int vocab_size = std::stoi(line.substr(6));
    std::vector<std::string> vocab;
    for (int i = 0; i < vocab_size; ++i) {
        std::getline(file, line);
        vocab.push_back(line);
    }

    Tfidf new_tfidf;
    new_tfidf.fit({vocab});
    tfidf_ = new_tfidf;

    std::getline(file, line);
    if (line.substr(0, 3) != "LR ") return false;

    std::istringstream lr_ss(line.substr(3));
    int lr_classes, lr_features;
    lr_ss >> lr_classes >> lr_features;

    LogisticRegression lr;
    std::vector<std::vector<double>> weights(lr_classes, std::vector<double>(lr_features));
    for (int i = 0; i < lr_classes; ++i) {
        std::getline(file, line);
        std::istringstream ws(line);
        for (int j = 0; j < lr_features; ++j) {
            ws >> weights[i][j];
        }
    }

    std::vector<double> bias(lr_classes);
    std::getline(file, line);
    std::istringstream bs(line);
    for (int i = 0; i < lr_classes; ++i) {
        bs >> bias[i];
    }

    lr.setWeights(weights);
    lr.setBias(bias);
    lr.setNumClasses(lr_classes);
    lr_ = lr;

    return true;
}

void Model::setTfidf(const Tfidf& tfidf) {
    tfidf_ = tfidf;
}

void Model::setLogisticRegression(const LogisticRegression& lr) {
    lr_ = lr;
}

void Model::setClasses(const std::vector<std::string>& classes) {
    classes_ = classes;
}

const Tfidf& Model::tfidf() const {
    return tfidf_;
}

const LogisticRegression& Model::logisticRegression() const {
    return lr_;
}

const std::vector<std::string>& Model::classes() const {
    return classes_;
}

}
