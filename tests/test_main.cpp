#include "test_main.hpp"
#include <iostream>

int main() {
    std::cout << "=== SecureLens Test Suite ===\n\n";

    run_parser_tests();
    std::cout << "\n";
    run_ml_tests();
    std::cout << "\n";
    run_security_tests();
    std::cout << "\n";
    run_api_tests();

    std::cout << "\n=== All Tests Complete ===\n";
    return 0;
}
