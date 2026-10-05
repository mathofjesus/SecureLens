#pragma once
#include "ml/tfidf.hpp"
#include "ml/logistic_regression.hpp"
#include <string>
#include <vector>

namespace securelens {

class Model {
public:
    bool save(const std::string& path) const;
    bool load(const std::string& path);

    void setTfidf(const Tfidf& tfidf);
    void setLogisticRegression(const LogisticRegression& lr);
    void setClasses(const std::vector<std::string>& classes);

    const Tfidf& tfidf() const;
    const LogisticRegression& logisticRegression() const;
    const std::vector<std::string>& classes() const;

private:
    Tfidf tfidf_;
    LogisticRegression lr_;
    std::vector<std::string> classes_;
};

}
