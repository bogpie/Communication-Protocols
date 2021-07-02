
#include "response_parsers.h"

using namespace std;
using nlohmann::json;

string parseCode(string response, int &code)
{
    int codeStart = response.find("HTTP/1.1 ") + strlen("HTTP/1.1 ");
    std::stringstream stream(response.substr(codeStart));
    stream >> code;
    return to_string(code);
}

string parseResponse(string response, bool &isError, json &receivedJson)
{
    string parsedResponse;
    int code = 0;
    parsedResponse = "Response code: " + parseCode(response, code) + "\n";

    // Look for a list of json objects (books), if not found, look for a single object
    size_t jsonStart = response.find("[");
    if (jsonStart != string::npos)
    {
        receivedJson = json::parse(response.substr(jsonStart));
    }
    else
    {
        jsonStart = response.find("{");
        if (jsonStart != string::npos)
        {
            receivedJson = json::parse(response.substr(jsonStart));
        }
    }

    // Interpret response code
    if (code == 201 || code == 200)
    {
        parsedResponse += "Success!\n";
        isError = false;
    }
    else
    {
        parsedResponse += "Error! ";
        isError = true;

        if (receivedJson != NULL)
        {
            string additionalInfo;
            parsedResponse += receivedJson["error"].get<string>() + "\n";
        }
        else
        {
            parsedResponse += "Too many requests\n";
        }
    }

    return parsedResponse;
}


string extractCookie(string response)
{
    int cookieStart = response.find("Set-Cookie: ") + strlen("Set-Cookie: ");
    int cookieEnd = response.find("HttpOnly") + strlen("HttpOnly");
    string loginCookies = response.substr(cookieStart, cookieEnd - cookieStart);
    return loginCookies;
}
