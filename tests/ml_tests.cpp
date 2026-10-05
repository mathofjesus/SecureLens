#include "ml/tokenizer.hpp"
#include "ml/tfidf.hpp"
#include "ml/logistic_regression.hpp"
#include "ml/model.hpp"
#include "ml/classifier.hpp"
#include <iostream>
#include <cmath>
#include <fstream>

using namespace securelens;

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) void test_##name()
#define RUN_TEST(name) do { \
    std::cout << "Running " #name "... "; \
    try { test_##name(); std::cout << "PASSED\n"; tests_passed++; } \
    catch (const std::exception& e) { std::cout << "FAILED: " << e.what() << "\n"; tests_failed++; } \
} while(0)

#define ASSERT_TRUE(expr) do { if (!(expr)) throw std::runtime_error("Assertion failed: " #expr); } while(0)
#define ASSERT_EQ(a, b) do { if ((a) != (b)) throw std::runtime_error("Assertion failed: " #a " == " #b); } while(0)
#define ASSERT_NEAR(a, b, eps) do { if (std::abs((a) - (b)) > (eps)) throw std::runtime_error("Assertion failed: " #a " ~= " #b); } while(0)

TEST(tokenizer_basic) {
    Tokenizer t;
    auto tokens = t.tokenize("Hello World");
    ASSERT_EQ(tokens.size(), 2);
    ASSERT_EQ(tokens[0], "hello");
    ASSERT_EQ(tokens[1], "world");
}

TEST(tokenizer_normalize) {
    Tokenizer t;
    auto tokens = t.tokenize("Hello, World!");
    ASSERT_EQ(tokens.size(), 2);
}

TEST(tokenizer_empty) {
    Tokenizer t;
    auto tokens = t.tokenize("");
    ASSERT_EQ(tokens.size(), 0);
}

TEST(tf_idf_basic) {
    Tfidf tfidf;
    std::vector<std::vector<std::string>> docs = {
        {"hello", "world"},
        {"hello", "there"}
    };
    tfidf.fit(docs);
    ASSERT_TRUE(tfidf.vocabSize() > 0);

    auto vec = tfidf.transform({"hello", "world"});
    ASSERT_EQ(vec.size(), static_cast<size_t>(tfidf.vocabSize()));
}

TEST(logistic_regression_basic) {
    LogisticRegression lr(0.1, 50, 0.01);
    std::vector<std::vector<double>> X = {{1.0, 0.0}, {0.0, 1.0}, {1.0, 1.0}, {0.0, 0.0}};
    std::vector<int> y = {0, 1, 1, 0};
    lr.fit(X, y, 2);
    ASSERT_EQ(lr.numClasses(), 2);

    auto probs = lr.predict_proba({1.0, 0.0});
    ASSERT_EQ(probs.size(), 2);
}

TEST(model_save_load) {
    Tfidf tfidf;
    std::vector<std::vector<std::string>> docs = {{"hello", "world"}, {"foo", "bar"}};
    tfidf.fit(docs);

    LogisticRegression lr(0.1, 10, 0.01);
    std::vector<std::vector<double>> X = {{1.0, 0.0}, {0.0, 1.0}};
    std::vector<int> y = {0, 1};
    lr.fit(X, y, 2);

    Model model;
    model.setTfidf(tfidf);
    model.setLogisticRegression(lr);
    model.setClasses({"negative", "positive"});

    ASSERT_TRUE(model.save("/tmp/test_model.txt"));

    Model loaded;
    ASSERT_TRUE(loaded.load("/tmp/test_model.txt"));
    ASSERT_EQ(loaded.classes().size(), 2);
}

TEST(classifier_basic) {
    Tfidf tfidf;
    std::vector<std::vector<std::string>> docs = {
        {"love", "amazing", "great"},
        {"terrible", "awful", "bad"},
        {"okay", "fine", "average"}
    };
    tfidf.fit(docs);

    LogisticRegression lr(0.1, 100, 0.01);
    std::vector<std::vector<double>> X;
    for (const auto& d : docs) X.push_back(tfidf.transform(d));
    std::vector<int> y = {2, 0, 1};
    lr.fit(X, y, 3);

    Model model;
    model.setTfidf(tfidf);
    model.setLogisticRegression(lr);
    model.setClasses({"negative", "neutral", "positive"});

    Classifier classifier(model);
    auto result = classifier.classify("love amazing great");
    ASSERT_TRUE(result.confidence > 0.0);
}

void run_ml_tests() {
    std::cout << "=== ML Tests ===\n";
    RUN_TEST(tokenizer_basic);
    RUN_TEST(tokenizer_normalize);
    RUN_TEST(tokenizer_empty);
    RUN_TEST(tf_idf_basic);
    RUN_TEST(logistic_regression_basic);
    RUN_TEST(model_save_load);
    RUN_TEST(classifier_basic);

    std::cout << "\n=== ML Results ===\n";
    std::cout << "Passed: " << tests_passed << "\n";
    std::cout << "Failed: " << tests_failed << "\n";
}
