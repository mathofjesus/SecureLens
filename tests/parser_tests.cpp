#include "security/parsers/requirements_parser.hpp"
#include "security/parsers/package_json_parser.hpp"
#include "security/parsers/pom_parser.hpp"
#include <iostream>
#include <cassert>
#include <fstream>
#include <filesystem>

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

TEST(requirements_parser_basic) {
    RequirementsParser parser;
    auto deps = parser.parse("requests==2.19.0\nflask==2.0.0\n");
    ASSERT_EQ(deps.size(), 2);
    ASSERT_EQ(deps[0].name, "requests");
    ASSERT_EQ(deps[0].version, "2.19.0");
    ASSERT_EQ(deps[0].ecosystem, "pypi");
}

TEST(requirements_parser_empty) {
    RequirementsParser parser;
    auto deps = parser.parse("");
    ASSERT_EQ(deps.size(), 0);
}

TEST(requirements_parser_comments) {
    RequirementsParser parser;
    auto deps = parser.parse("# comment\nrequests==2.19.0\n");
    ASSERT_EQ(deps.size(), 1);
}

TEST(package_json_parser_basic) {
    PackageJsonParser parser;
    std::string json = R"({"dependencies":{"lodash":"4.17.21"},"devDependencies":{"jest":"29.0.0"}})";
    auto deps = parser.parse(json);
    ASSERT_EQ(deps.size(), 2);
}

TEST(package_json_parser_invalid) {
    PackageJsonParser parser;
    auto deps = parser.parse("not json");
    ASSERT_EQ(deps.size(), 0);
}

TEST(pom_parser_basic) {
    PomParser parser;
    std::string xml = R"(<project><dependencies><dependency><groupId>org.springframework</groupId><artifactId>spring-core</artifactId><version>5.3.0</version></dependency></dependencies></project>)";
    auto deps = parser.parse(xml);
    ASSERT_EQ(deps.size(), 1);
    ASSERT_EQ(deps[0].artifact, "spring-core");
    ASSERT_EQ(deps[0].version, "5.3.0");
}

TEST(pom_parser_invalid) {
    PomParser parser;
    auto deps = parser.parse("not xml");
    ASSERT_EQ(deps.size(), 0);
}

void run_parser_tests() {
    std::cout << "=== Parser Tests ===\n";
    RUN_TEST(requirements_parser_basic);
    RUN_TEST(requirements_parser_empty);
    RUN_TEST(requirements_parser_comments);
    RUN_TEST(package_json_parser_basic);
    RUN_TEST(package_json_parser_invalid);
    RUN_TEST(pom_parser_basic);
    RUN_TEST(pom_parser_invalid);

    std::cout << "\n=== Parser Results ===\n";
    std::cout << "Passed: " << tests_passed << "\n";
    std::cout << "Failed: " << tests_failed << "\n";
}
