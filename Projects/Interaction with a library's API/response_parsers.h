#ifndef RESPONSEPARSERS
#define RESPONSEPARSERS

#include "helpers.h"

// Get error / success code from response
std::string parseCode(std::string response, int &code);

// Parse validity of query and received json (if exists) from response
std::string parseResponse(std::string response, bool &isError, nlohmann::json &receivedJson);

// Get cookie from response
std::string extractCookie(std::string response);

#endif