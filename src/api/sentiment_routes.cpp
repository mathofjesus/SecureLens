#include "api/sentiment_routes.hpp"
#include "ml/classifier.hpp"
#include "ml/model.hpp"
#include <nlohmann/json.hpp>
#include <fstream>

namespace securelens {

using json = nlohmann::json;

std::string handleSentiment(const std::string& body) {
    try {
        auto j = json::parse(body);

        if (!j.contains("text")) {
            return "{\"error\":\"Missing 'text' field\"}";
        }

        std::string text = j["text"].get<std::string>();
        if (text.empty()) {
            return "{\"error\":\"Text cannot be empty\"}";
        }
        if (text.length() > 10000) {
            return "{\"error\":\"Text too long (max 10000 chars)\"}";
        }

        Model model;
        std::ifstream model_file("models/sentiment_model.txt");
        if (!model_file) {
            return "{\"error\":\"Model not found. Run securelens-train first.\"}";
        }
        model_file.close();

        if (!model.load("models/sentiment_model.txt")) {
            return "{\"error\":\"Failed to load model\"}";
        }

        Classifier classifier(model);
        auto result = classifier.classify(text);

        json response;
        response["sentiment"] = result.sentiment;
        response["confidence"] = result.confidence;

        return response.dump();
    } catch (const json::exception& e) {
        return std::string("{\"error\":\"Invalid JSON: ") + e.what() + "\"}";
    } catch (const std::exception& e) {
        return std::string("{\"error\":\"") + e.what() + "\"}";
    }
}

}
