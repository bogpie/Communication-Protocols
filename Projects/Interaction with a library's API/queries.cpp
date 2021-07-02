
#include "queries.h"

using namespace std;
using nlohmann::json;

// Try registering and print a warning if user is already logged in
void tryRegister(string host, int socket, json payload, string message,
                 char *messageArr, char *responseArr, string loginCookies)
{
    printLoggedIn(loginCookies);

    // Gather credentials from user
    getCredentials(payload);

    // Create message to be sent to server
    message = computePostRequest(host, REGISTER_URL, JSON_TYPE, payload.dump(), "", "");

    // Send message to server
    strcpy(messageArr, message.c_str());
    send_to_server(socket, messageArr);

    // Receive response from server
    responseArr = receive_from_server(socket);
    string response(responseArr);

    // Parse response, possibly find error
    bool isError;
    json receivedJson = NULL;
    string parsedResponse = parseResponse(response, isError, receivedJson);
    // cout << "The response is:\n"
    cout << parsedResponse << "\n";
    if (isError)
        return;
}


// Try logging in and print a warning if user is already logged in; provides login cookies
void tryLogin(string host, int socket, json payload, string message,
              char *messageArr, char *responseArr, string &loginCookies)
{
    printLoggedIn(loginCookies);

    // Gather credentials from user
    getCredentials(payload);

    // Create message to be sent to server
    message = computePostRequest(host, LOGIN_URL, JSON_TYPE, payload.dump(), "", "");

    // Send message to server
    strcpy(messageArr, message.c_str());
    send_to_server(socket, messageArr);

    // Receive response from server
    responseArr = receive_from_server(socket);
    string response(responseArr);

    bool isError;
    json receivedJson = NULL;
    string parsedResponse = parseResponse(response, isError, receivedJson);

    cout << parsedResponse << "\n";
    if (isError)
        return;

    loginCookies = extractCookie(response);
}


// Try logging out and print a warning if user is not logged in. Updates login cookies and access token.
void tryLogout(string host, int socket, json payload, string message,
               char *messageArr, char *responseArr, string &loginCookies, string &accessToken)
{
    printLoggedOut(loginCookies);

    // Create message to be sent to server
    message = computeGetDeleteRequest("GET", host, LOGOUT_URL, loginCookies, "");

    // Send message to server
    strcpy(messageArr, message.c_str());
    send_to_server(socket, messageArr);

    // Receive response from server
    responseArr = receive_from_server(socket);
    string response(responseArr);
    bool isError;
    json receivedJson = NULL;
    string parsedResponse = parseResponse(response, isError, receivedJson);

    cout << parsedResponse << "\n";

    if (isError)
        return;

    loginCookies.clear();
    accessToken.clear();
}

// Try getting access to library. Updates access token
void tryEnterLibrary(string host, int socket, json payload, string message,
                     char *messageArr, char *responseArr, string loginCookies, string &accessToken)
{
    printLoggedOut(loginCookies);

    // Create message to be sent to server
    message = computeGetDeleteRequest("GET", host, ENTER_LIBRARY_URL, loginCookies, "");

    // Send message to server
    strcpy(messageArr, message.c_str());
    send_to_server(socket, messageArr);

    // Receive response from server
    responseArr = receive_from_server(socket);
    string response(responseArr);
    bool isError;
    json receivedJson = NULL;
    string parsedResponse = parseResponse(response, isError, receivedJson);

    cout << parsedResponse << "\n";

    if (isError)
        return;

    accessToken = receivedJson["token"].get<string>();
    cout << "The access token for library is: <" << accessToken << ">\n\n";
}


// Try getting or deleting books based on provided command.
void tryBooksQuery(string command, string host, int socket, json payload, string message, char *messageArr,
                  char *responseArr, string loginCookies, string accessToken)
{
    printLoggedOut(loginCookies);
    printNoAccess(accessToken);

    int id = 0;
    bool usesID = !command.compare("get_book") || !command.compare("delete_book");
    string method = (!command.compare("delete_book")) ? "DELETE" : "GET";

    if (usesID)
    {
        cout << "id=";
        cin >> id;
    }

    // Create message to be sent to server
    string url = BOOKS_URL + (usesID ? "/" + to_string(id) : "");
    message = computeGetDeleteRequest(method, host, url, loginCookies,
                                      accessToken);

    // Send message to server
    strcpy(messageArr, message.c_str());
    send_to_server(socket, messageArr);

    // Receive response from server
    responseArr = receive_from_server(socket);
    string response(responseArr);

    // Print brief version of response
    bool isError;
    json receivedJson = NULL;
    string parsedResponse = parseResponse(response, isError, receivedJson);
    cout << parsedResponse << "\n";

    // Do necessary operations using json
    if (isError)
        return;
    if (receivedJson != NULL)
    {
        for (json::iterator bookIterator = receivedJson.begin(); bookIterator != receivedJson.end(); ++bookIterator)
        {
            json bookObject = *bookIterator;

            id = usesID ? id : bookObject["id"].get<int>();

            if (!id)
                continue;

            cout << "Book no. " << id << "\n";

            for (json::iterator fieldIterator = bookObject.begin(); fieldIterator != bookObject.end(); ++fieldIterator)
            {
                if (fieldIterator.key() == "id")
                {
                    continue;
                }

                cout << fieldIterator.key() << " is " << fieldIterator.value() << "\n";
            }
            cout << "\n";
        }
    }
}

// Try adding book.
void tryAddBook(string host, int socket, json payload, string message, char *messageArr, char *responseArr,
                string loginCookies, string accessToken)
{
    printLoggedOut(loginCookies);
    printNoAccess(accessToken);

    getBookInfo(payload);

    // Create message to be sent to server, providing an access token; no need for login cookie
    message = computePostRequest(host, BOOKS_URL, JSON_TYPE, payload.dump(), "", accessToken);

    // Send message to server
    strcpy(messageArr, message.c_str());
    send_to_server(socket, messageArr);

    // Receive response from server
    responseArr = receive_from_server(socket);

    string response(responseArr);

    bool isError;
    json receivedJson = NULL;

    // cout << response << "\n";

    string parsedResponse = parseResponse(response, isError, receivedJson);
    cout << parsedResponse << "\n";

    if (isError)
    {
        return;
    }
}

