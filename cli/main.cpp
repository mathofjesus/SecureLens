#include "security/scanner.hpp"
#include "ml/classifier.hpp"
#include "ml/model.hpp"
#include <iostream>
#include <fstream>
#include <string>

using namespace securelens;

static void printUsage() {
    std::cout << "Usage:\n";
    std::cout << "  securelens scan <path>       - Scan a directory for vulnerabilities\n";
    std::cout << "  securelens sentiment <text>  - Classify sentiment of text\n";
    std::cout << "  securelens analyze <path> <text> - Scan + sentiment analysis\n";
}

static int cmdScan(const std::string& path) {
    Scanner scanner;
    auto result = scanner.scanDirectory(path);

    std::cout << "Repository: " << result.repository << "\n";
    std::cout << "Dependencies: " << result.dependencies << "\n";
    std::cout << "Vulnerabilities: " << result.vulnerabilities << "\n";

    for (const auto& v : result.findings) {
        std::cout << "  [" << v.severity << "] " << v.id << " - " << v.summary << "\n";
        if (!v.fixed_version.empty()) {
            std::cout << "    Fixed in: " << v.fixed_version << "\n";
        }
    }

    return 0;
}

static int cmdSentiment(const std::string& text) {
    Model model;
    if (!model.load("models/sentiment_model.txt")) {
        std::cerr << "Error: Model not found. Run securelens-train first.\n";
        return 1;
    }

    Classifier classifier(model);
    auto result = classifier.classify(text);

    std::cout << "Sentiment: " << result.sentiment << "\n";
    std::cout << "Confidence: " << result.confidence << "\n";

    return 0;
}

static int cmdAnalyze(const std::string& path, const std::string& text) {
    Scanner scanner;
    auto scan_result = scanner.scanDirectory(path);

    std::cout << "=== Security Scan ===\n";
    std::cout << "Dependencies: " << scan_result.dependencies << "\n";
    std::cout << "Vulnerabilities: " << scan_result.vulnerabilities << "\n";

    for (const auto& v : scan_result.findings) {
        std::cout << "  [" << v.severity << "] " << v.id << " - " << v.summary << "\n";
    }

    std::cout << "\n=== Sentiment Analysis ===\n";

    Model model;
    if (!model.load("models/sentiment_model.txt")) {
        std::cerr << "Error: Model not found. Run securelens-train first.\n";
        return 1;
    }

    Classifier classifier(model);
    auto sent_result = classifier.classify(text);

    std::cout << "Sentiment: " << sent_result.sentiment << "\n";
    std::cout << "Confidence: " << sent_result.confidence << "\n";

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }

    std::string cmd = argv[1];

    if (cmd == "scan" && argc >= 3) {
        return cmdScan(argv[2]);
    } else if (cmd == "sentiment" && argc >= 3) {
        return cmdSentiment(argv[2]);
    } else if (cmd == "analyze" && argc >= 4) {
        return cmdAnalyze(argv[2], argv[3]);
    } else {
        printUsage();
        return 1;
    }
}
