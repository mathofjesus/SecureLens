#pragma once
#include <vector>
#include <string>

namespace securelens {

class LogisticRegression {
public:
    LogisticRegression(double learning_rate = 0.1, int epochs = 100, double regularization = 0.01);

    void fit(const std::vector<std::vector<double>>& X, const std::vector<int>& y, int num_classes);
    std::vector<double> predict_proba(const std::vector<double>& x) const;
    int predict(const std::vector<double>& x) const;

    const std::vector<std::vector<double>>& weights() const;
    const std::vector<double>& bias() const;
    int numClasses() const;

    void setWeights(const std::vector<std::vector<double>>& w);
    void setBias(const std::vector<double>& b);
    void setNumClasses(int n);

private:
    double learning_rate_;
    int epochs_;
    double regularization_;
    std::vector<std::vector<double>> weights_;
    std::vector<double> bias_;
    int num_classes_ = 0;
};

}
