#include "ml/model.hpp"
#include "ml/classifier.hpp"
#include <iostream>
#include <string>

using namespace securelens;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: securelens-evaluate <model_path>\n";
        return 1;
    }

    Model model;
    if (!model.load(argv[1])) {
        std::cerr << "Error: Failed to load model from " << argv[1] << "\n";
        return 1;
    }

    std::cout << "Model loaded successfully\n";
    std::cout << "Classes: ";
    for (const auto& c : model.classes()) std::cout << c << " ";
    std::cout << "\n";

    Classifier classifier(model);

    std::vector<std::string> test_texts = {
        "I love this product, it is amazing",
        "This is okay, nothing special",
        "Terrible experience, would not recommend"
    };

    for (const auto& text : test_texts) {
        auto result = classifier.classify(text);
        std::cout << "Text: " << text << "\n";
        std::cout << "  Sentiment: " << result.sentiment << "\n";
        std::cout << "  Confidence: " << result.confidence << "\n\n";
    }

    return 0;
}
