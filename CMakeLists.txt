cmake_minimum_required(VERSION 3.16)
project(SecureLens VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG -Wall -Wextra")

find_package(CURL REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(tinyxml2 REQUIRED)

add_library(securelens_core
    src/security/dependency.cpp
    src/security/parsers/requirements_parser.cpp
    src/security/parsers/package_json_parser.cpp
    src/security/parsers/pom_parser.cpp
    src/security/osv_client.cpp
    src/security/scanner.cpp
    src/ml/tokenizer.cpp
    src/ml/tfidf.cpp
    src/ml/logistic_regression.cpp
    src/ml/model.cpp
    src/ml/classifier.cpp
    src/api/security_routes.cpp
    src/api/sentiment_routes.cpp
)

target_include_directories(securelens_core PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(securelens_core PUBLIC CURL::libcurl nlohmann_json::nlohmann_json tinyxml2)

add_executable(securelens src/main.cpp)
target_link_libraries(securelens PRIVATE securelens_core)

add_executable(securelens-train training/train.cpp)
target_link_libraries(securelens-train PRIVATE securelens_core)

add_executable(securelens-cli cli/main.cpp)
target_link_libraries(securelens-cli PRIVATE securelens_core)

enable_testing()
add_executable(securelens-test
    tests/test_main.cpp
    tests/parser_tests.cpp
    tests/ml_tests.cpp
    tests/security_tests.cpp
    tests/api_tests.cpp
)
target_link_libraries(securelens-test PRIVATE securelens_core)
add_test(NAME securelens_tests COMMAND securelens-test)
