#pragma once

#include <cctype> // used for lowercase
#include <iostream>
#include <string>
#include <thread>
#include <algorithm>

#include "../globals.h"

#if OCERO_ENABLE_LLM
#include <json.hpp>
#include <curl/curl.h>
#endif


size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response);

std::string sendOllamaRequest(const std::string& model, const std::string& prompt);
void sendPythonRequest(const std::string& prompt);
void runRequestInThread(const std::string& prompt);
