#include "ml/logistic_regression.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace securelens {

LogisticRegression::LogisticRegression(double learning_rate, int epochs, double regularization)
    : learning_rate_(learning_rate), epochs_(epochs), regularization_(regularization) {}

static std::vector<double> softmax(const std::vector<double>& logits) {
    std::vector<double> exp_logits(logits.size());
    double max_logit = *std::max_element(logits.begin(), logits.end());
    double sum = 0.0;

    for (size_t i = 0; i < logits.size(); ++i) {
        exp_logits[i] = std::exp(logits[i] - max_logit);
        sum += exp_logits[i];
    }

    for (auto& val : exp_logits) {
        val /= sum;
    }

    return exp_logits;
}

void LogisticRegression::fit(const std::vector<std::vector<double>>& X,
                              const std::vector<int>& y,
                              int num_classes) {
    if (X.empty() || y.empty() || num_classes <= 0) return;

    int n_features = static_cast<int>(X[0].size());
    num_classes_ = num_classes;

    weights_.assign(num_classes, std::vector<double>(n_features, 0.0));
    bias_.assign(num_classes, 0.0);

    std::mt19937 gen(42);
    std::uniform_real_distribution<> dis(-0.01, 0.01);

    for (auto& row : weights_) {
        for (auto& w : row) {
            w = dis(gen);
        }
    }

    int n_samples = static_cast<int>(X.size());

    for (int epoch = 0; epoch < epochs_; ++epoch) {
        for (int i = 0; i < n_samples; ++i) {
            std::vector<double> logits(num_classes_, 0.0);
            for (int c = 0; c < num_classes_; ++c) {
                for (int f = 0; f < n_features; ++f) {
                    logits[c] += weights_[c][f] * X[i][f];
                }
                logits[c] += bias_[c];
            }

            auto probs = softmax(logits);

            for (int c = 0; c < num_classes_; ++c) {
                double error = (c == y[i] ? 1.0 : 0.0) - probs[c];
                for (int f = 0; f < n_features; ++f) {
                    weights_[c][f] += learning_rate_ * (error * X[i][f] - regularization_ * weights_[c][f]);
                }
                bias_[c] += learning_rate_ * error;
            }
        }
    }
}

std::vector<double> LogisticRegression::predict_proba(const std::vector<double>& x) const {
    if (weights_.empty()) return {};

    std::vector<double> logits(num_classes_, 0.0);
    for (int c = 0; c < num_classes_; ++c) {
        for (size_t f = 0; f < x.size(); ++f) {
            logits[c] += weights_[c][f] * x[f];
        }
        logits[c] += bias_[c];
    }

    return softmax(logits);
}

int LogisticRegression::predict(const std::vector<double>& x) const {
    auto probs = predict_proba(x);
    if (probs.empty()) return -1;

    return static_cast<int>(std::max_element(probs.begin(), probs.end()) - probs.begin());
}

const std::vector<std::vector<double>>& LogisticRegression::weights() const {
    return weights_;
}

const std::vector<double>& LogisticRegression::bias() const {
    return bias_;
}

int LogisticRegression::numClasses() const {
    return num_classes_;
}

void LogisticRegression::setWeights(const std::vector<std::vector<double>>& w) {
    weights_ = w;
}

void LogisticRegression::setBias(const std::vector<double>& b) {
    bias_ = b;
}

void LogisticRegression::setNumClasses(int n) {
    num_classes_ = n;
}

}
