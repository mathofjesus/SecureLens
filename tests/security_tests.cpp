#include "security/scanner.hpp"
#include "security/osv_client.hpp"
#include <iostream>
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

TEST(scanner_empty_dir) {
    std::string tmpdir = "/tmp/securelens_test_empty";
    std::filesystem::create_directories(tmpdir);

    Scanner scanner;
    auto result = scanner.scanDirectory(tmpdir);
    ASSERT_EQ(result.dependencies, 0);
    ASSERT_EQ(result.vulnerabilities, 0);

    std::filesystem::remove_all(tmpdir);
}

TEST(scanner_requirements_txt) {
    std::string tmpdir = "/tmp/securelens_test_req";
    std::filesystem::create_directories(tmpdir);

    std::ofstream file(tmpdir + "/requirements.txt");
    file << "requests==2.19.0\nflask==2.0.0\n";
    file.close();

    Scanner scanner;
    auto result = scanner.scanDirectory(tmpdir);
    ASSERT_EQ(result.dependencies, 2);

    std::filesystem::remove_all(tmpdir);
}

TEST(scanner_package_json) {
    std::string tmpdir = "/tmp/securelens_test_pkg";
    std::filesystem::create_directories(tmpdir);

    std::ofstream file(tmpdir + "/package.json");
    file << R"({"dependencies":{"lodash":"4.17.21"},"devDependencies":{"jest":"29.0.0"}})";
    file.close();

    Scanner scanner;
    auto result = scanner.scanDirectory(tmpdir);
    ASSERT_EQ(result.dependencies, 2);

    std::filesystem::remove_all(tmpdir);
}

TEST(scanner_pom_xml) {
    std::string tmpdir = "/tmp/securelens_test_pom";
    std::filesystem::create_directories(tmpdir);

    std::ofstream file(tmpdir + "/pom.xml");
    file << R"(<project><dependencies><dependency><groupId>org.springframework</groupId><artifactId>spring-core</artifactId><version>5.3.0</version></dependency></dependencies></project>)";
    file.close();

    Scanner scanner;
    auto result = scanner.scanDirectory(tmpdir);
    ASSERT_EQ(result.dependencies, 1);

    std::filesystem::remove_all(tmpdir);
}

TEST(scanner_nonexistent_dir) {
    Scanner scanner;
    auto result = scanner.scanDirectory("/nonexistent/path/that/does/not/exist");
    ASSERT_EQ(result.dependencies, 0);
}

TEST(osv_client_invalid) {
    OSVClient client;
    auto vulns = client.query("invalid_ecosystem", "invalid_package", "0.0.0");
    ASSERT_EQ(vulns.size(), 0);
}

void run_security_tests() {
    std::cout << "=== Security Tests ===\n";
    RUN_TEST(scanner_empty_dir);
    RUN_TEST(scanner_requirements_txt);
    RUN_TEST(scanner_package_json);
    RUN_TEST(scanner_pom_xml);
    RUN_TEST(scanner_nonexistent_dir);
    RUN_TEST(osv_client_invalid);

    std::cout << "\n=== Security Results ===\n";
    std::cout << "Passed: " << tests_passed << "\n";
    std::cout << "Failed: " << tests_failed << "\n";
}
