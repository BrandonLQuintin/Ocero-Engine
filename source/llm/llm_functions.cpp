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

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            response = ""; // Clear the response in case of an error
        }

        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    } else {
        std::cerr << "Failed to initialize CURL" << std::endl;
    }

    nlohmann::json jsonData = nlohmann::json::parse(response);

    response = jsonData["response"];

    std::transform(response.begin(), response.end(), response.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    response = "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\" + response;

    return response;
}
