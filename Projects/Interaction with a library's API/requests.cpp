
#include "requests.h"

using namespace std;
using nlohmann::json;

std::string computePostRequest(std::string host, std::string url, std::string contentType, std::string payload,
                               string loginCookie, string accessToken)
{
    // Add request type, URL link, protocol
    string message;
    string line;
    line = "POST " + url + " HTTP/1.1";
    computeMessage(message, line);

    // Mention host data
    line = "Host: " + host;
    computeMessage(message, line);

    // Content-Type
    line = "Content-Type: " + contentType;
    computeMessage(message, line);

    // Content-Length
    line = "Content-Length: " + to_string(payload.length());
    computeMessage(message, line);

    // Add auth header (if it exists)
    if (!accessToken.empty())
    {
        line = "Authorization: Bearer " + accessToken;
        computeMessage(message, line);
    }

    // Add login cookie (if it exists)
    if (!loginCookie.empty())
    {
        line = "Cookie: " + loginCookie;
        computeMessage(message, line);
    }

    // Append content to message
    computeMessage(message, "");
    computeMessage(message, payload);

    return message;
}

std::string computeGetDeleteRequest(std::string method, std::string host, std::string url, string loginCookie, string accessToken)
{
    // Add request type, URL link, protocol
    string message;
    string line;
    line = method + " " + url + " HTTP/1.1";
    computeMessage(message, line);

    // Mention host data
    line = "Host: " + host;
    computeMessage(message, line);

    // Add auth header (if it exists)
    if (!accessToken.empty())
    {
        line = "Authorization: Bearer " + accessToken;
        computeMessage(message, line);
    }

    // Add login cookie (if it exists)
    if (!loginCookie.empty())
    {
        line = "Cookie: " + loginCookie;
        computeMessage(message, line);
    }

    // End message with new line
    computeMessage(message, "");

    return message;
}
