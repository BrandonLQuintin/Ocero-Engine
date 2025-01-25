#include "llm_functions.h"

// Callback function to capture the response from the server
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t totalSize = size * nmemb;
    response->append((char*)contents, totalSize);
    return totalSize;
}

std::string sendOllamaRequest(const std::string& model, const std::string& prompt) {
    const std::string url = "http://localhost:11434/api/generate";

    // JSON payload for the request
    std::string jsonPayload = R"({
        "model": ")" + model + R"(",
        "prompt": ")" + prompt + R"(",
        "stream": false
    })";

    CURL* curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (curl) {
        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set headers
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the JSON payload
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());

        // Set the write callback to capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Set a timeout for the request
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            if (res == CURLE_COULDNT_CONNECT) {
                std::cerr << "Failed to connect to Ollama server. Is it running on localhost:11434?" << std::endl;
            } else {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            }
            response = ""; // Clear the response in case of an error
        } else {
            // Parse the response
            try {
                nlohmann::json jsonData = nlohmann::json::parse(response);
                response = jsonData["response"];
                std::transform(response.begin(), response.end(), response.begin(),
                               [](unsigned char c) { return std::tolower(c); });
                response = "\\\\\\\\\\\\\\\\\\\\\\\\\\" + response;
            } catch (const std::exception& e) {
                response = "\\\\\\\\\\\\\\\\\\\\\\\\\\cannot connect to ollama, please download and run ollama from ollama.com";
                std::cerr << "Failed to parse JSON response: " << e.what() << std::endl;
            }
        }

        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    } else {
        std::cerr << "Failed to initialize CURL" << std::endl;
    }

    return response;
}


std::string sendPythonRequest(const std::string& prompt) {
    const std::string url = "http://localhost:5000/chat";

    std::string jsonPayload = R"({
        "prompt": ")" + prompt + R"("
    })";

    CURL* curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonPayload.c_str());

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        try {
            nlohmann::json jsonData = nlohmann::json::parse(response);
            response = jsonData["response"];
            std::transform(response.begin(), response.end(), response.begin(),
                            [](unsigned char c) { return std::tolower(c); });
            response = "\\\\\\\\\\\\\\\\\\\\\\\\\\" + response;
        } catch (const std::exception& e) {
            std::cerr << "Failed to parse JSON response: " << e.what() << std::endl;
            response = "\\\\\\\\\\\\\\\\\\\\\\\\\\cannot connect to ollama, please download and run ollama from ollama.com";
        }
    }
    const size_t maxLength = 300;
    if (response.length() > maxLength) {
        response = response.substr(0, maxLength); // Get only the first maxLength characters
        std::cout << response << std::endl;
    }
    return response;
}
