#include "ml/tokenizer.hpp"
#include "ml/tfidf.hpp"
#include "ml/logistic_regression.hpp"
#include "ml/model.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <cmath>

using namespace securelens;

struct Dataset {
    std::vector<std::string> texts;
    std::vector<int> labels;
    std::vector<std::string> classes;
};

static Dataset loadDataset(const std::string& path) {
    Dataset ds;
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Error: Cannot open dataset: " << path << "\n";
        return ds;
    }

    std::string line;
    std::getline(file, line);

    while (std::getline(file, line)) {
        auto pos = line.rfind(',');
        if (pos == std::string::npos) continue;

        std::string text = line.substr(0, pos);
        std::string label = line.substr(pos + 1);

        if (text.empty() || label.empty()) continue;

        int label_idx = -1;
        for (size_t i = 0; i < ds.classes.size(); ++i) {
            if (ds.classes[i] == label) {
                label_idx = static_cast<int>(i);
                break;
            }
        }
        if (label_idx == -1) {
            label_idx = static_cast<int>(ds.classes.size());
            ds.classes.push_back(label);
        }

        ds.texts.push_back(text);
        ds.labels.push_back(label_idx);
    }

    return ds;
}

static void printMetrics(const std::vector<int>& y_true, const std::vector<int>& y_pred, int num_classes) {
    std::vector<std::vector<int>> cm(num_classes, std::vector<int>(num_classes, 0));
    int correct = 0;
    int n = static_cast<int>(y_true.size());

    for (int i = 0; i < n; ++i) {
        cm[y_true[i]][y_pred[i]]++;
        if (y_true[i] == y_pred[i]) correct++;
    }

    double accuracy = n > 0 ? static_cast<double>(correct) / n : 0.0;
    std::cout << "Accuracy: " << accuracy << "\n";

    for (int c = 0; c < num_classes; ++c) {
        int tp = cm[c][c];
        int fp = 0, fn = 0;
        for (int i = 0; i < num_classes; ++i) {
            if (i != c) {
                fp += cm[i][c];
                fn += cm[c][i];
            }
        }

        double precision = (tp + fp) > 0 ? static_cast<double>(tp) / (tp + fp) : 0.0;
        double recall = (tp + fn) > 0 ? static_cast<double>(tp) / (tp + fn) : 0.0;
        double f1 = (precision + recall) > 0 ? 2.0 * precision * recall / (precision + recall) : 0.0;

        std::cout << "Class " << c << ": Precision=" << precision
                  << " Recall=" << recall << " F1=" << f1 << "\n";
    }

    std::cout << "Confusion Matrix:\n";
    for (int i = 0; i < num_classes; ++i) {
        for (int j = 0; j < num_classes; ++j) {
            std::cout << cm[i][j] << " ";
        }
        std::cout << "\n";
    }
}

int main() {
    std::cout << "SecureLens Training\n";
    std::cout << "====================\n";

    auto ds = loadDataset("training/dataset/sentiment.csv");
    if (ds.texts.empty()) {
        std::cerr << "Error: Empty dataset\n";
        return 1;
    }

    std::cout << "Loaded " << ds.texts.size() << " samples\n";
    std::cout << "Classes: ";
    for (const auto& c : ds.classes) std::cout << c << " ";
    std::cout << "\n\n";

    Tokenizer tokenizer;
    std::vector<std::vector<std::string>> tokenized;
    for (const auto& text : ds.texts) {
        tokenized.push_back(tokenizer.tokenize(text));
    }

    Tfidf tfidf;
    tfidf.fit(tokenized);
    std::cout << "Vocabulary size: " << tfidf.vocabSize() << "\n\n";

    std::vector<std::vector<double>> X;
    for (const auto& tokens : tokenized) {
        X.push_back(tfidf.transform(tokens));
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::vector<int> indices(ds.texts.size());
    for (size_t i = 0; i < indices.size(); ++i) indices[i] = static_cast<int>(i);
    std::shuffle(indices.begin(), indices.end(), g);

    int train_size = static_cast<int>(ds.texts.size() * 0.8);
    std::vector<std::vector<double>> X_train, X_test;
    std::vector<int> y_train, y_test;

    for (int i = 0; i < train_size; ++i) {
        X_train.push_back(X[indices[i]]);
        y_train.push_back(ds.labels[indices[i]]);
    }
    for (size_t i = train_size; i < ds.texts.size(); ++i) {
        X_test.push_back(X[indices[i]]);
        y_test.push_back(ds.labels[indices[i]]);
    }

    LogisticRegression lr(0.1, 200, 0.01);
    lr.fit(X_train, y_train, static_cast<int>(ds.classes.size()));

    std::cout << "=== Training Metrics ===\n";
    std::vector<int> y_pred_train;
    for (const auto& x : X_train) {
        y_pred_train.push_back(lr.predict(x));
    }
    printMetrics(y_train, y_pred_train, static_cast<int>(ds.classes.size()));

    std::cout << "\n=== Test Metrics ===\n";
    std::vector<int> y_pred_test;
    for (const auto& x : X_test) {
        y_pred_test.push_back(lr.predict(x));
    }
    printMetrics(y_test, y_pred_test, static_cast<int>(ds.classes.size()));

    Model model;
    model.setTfidf(tfidf);
    model.setLogisticRegression(lr);
    model.setClasses(ds.classes);

    if (!model.save("models/sentiment_model.txt")) {
        std::cerr << "Error: Failed to save model\n";
        return 1;
    }

    std::cout << "\nModel saved to models/sentiment_model.txt\n";
    return 0;
}
