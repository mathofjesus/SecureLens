#include "api/security_routes.hpp"
#include "api/sentiment_routes.hpp"
#include <iostream>
#include <string>

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

TEST(security_scan_invalid_json) {
    auto result = handleSecurityScan("not json");
    ASSERT_TRUE(result.find("error") != std::string::npos);
}

TEST(security_scan_missing_fields) {
    auto result = handleSecurityScan(R"({})");
    ASSERT_TRUE(result.find("error") != std::string::npos);
}

TEST(security_scan_empty_path) {
    auto result = handleSecurityScan(R"({"path":"/nonexistent/path"})");
    ASSERT_TRUE(result.find("dependencies") != std::string::npos);
}

TEST(sentiment_missing_text) {
    auto result = handleSentiment(R"({})");
    ASSERT_TRUE(result.find("error") != std::string::npos);
}

TEST(sentiment_empty_text) {
    auto result = handleSentiment(R"({"text":""})");
    ASSERT_TRUE(result.find("error") != std::string::npos);
}

TEST(sentiment_no_model) {
    auto result = handleSentiment(R"({"text":"hello world"})");
    ASSERT_TRUE(result.find("error") != std::string::npos || result.find("Model") != std::string::npos);
}

void run_api_tests() {
    std::cout << "=== API Tests ===\n";
    RUN_TEST(security_scan_invalid_json);
    RUN_TEST(security_scan_missing_fields);
    RUN_TEST(security_scan_empty_path);
    RUN_TEST(sentiment_missing_text);
    RUN_TEST(sentiment_empty_text);
    RUN_TEST(sentiment_no_model);

    std::cout << "\n=== API Results ===\n";
    std::cout << "Passed: " << tests_passed << "\n";
    std::cout << "Failed: " << tests_failed << "\n";
}
