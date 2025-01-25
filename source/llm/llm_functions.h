#pragma once

#include <cctype> // used for lowercase
#include <json.hpp>
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <thread>
#include <algorithm>

#include "../globals.h"


size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response);

std::string sendOllamaRequest(const std::string& model, const std::string& prompt);
void sendPythonRequest(const std::string& prompt);
void runRequestInThread(const std::string& prompt);
