#include "helpers.h"
#include "requests.h"
#include "warnings.h"
#include "user_interaction.h"
#include "response_parsers.h"
#include "queries.h"

using namespace std;
using nlohmann::json;

int main(int argc, char *argv[])
{
    // Define server host
    char *hostArr = (char *)malloc(BUFLEN);
    strcpy(hostArr, "34.118.48.238");
    std::string host(hostArr);

    // Declare needed variables for server interaction
    json payload;
    string command = "";
    string message = "";
    string loginCookies = "";
    string accessToken = "";
    char *messageArr = (char *)malloc(BUFLEN);
    char *responseArr = (char *)malloc(BUFLEN);

    // Run client continuously, until exitting
    bool running = true;
    while (running)
    {
        payload.clear();
        command.clear();
        message.clear();
        fflush(stdin);

        // Avoid implicit length warning
        int sizeofMessage = sizeof(messageArr);
        int sizeofResponse = sizeof(responseArr);
        memset(messageArr, 0, sizeofMessage);
        memset(responseArr, 0, sizeofResponse);

        // Connect with server only after getting the command (avoid timeout)
        getline(cin, command);
        int socket = open_connection(hostArr, PORT, AF_INET, SOCK_STREAM, FLAG);

        // Interpret query
        if (!command.compare("register"))
        {
            tryRegister(host, socket, payload, message, messageArr, responseArr, loginCookies);
        }
        else if (!command.compare("login"))
        {
            tryLogin(host, socket, payload, message, messageArr, responseArr, loginCookies);
        }
        else if (!command.compare("logout"))
        {
            tryLogout(host, socket, payload, message, messageArr, responseArr, loginCookies, accessToken);
        }
        else if (!command.compare("enter_library"))
        {
            tryEnterLibrary(host, socket, payload, message, messageArr, responseArr, loginCookies, accessToken);
        }
        else if (!command.compare("get_books"))
        {
            tryBooksQuery(command, host, socket, payload, message, messageArr, responseArr, loginCookies, accessToken);
        }
        else if (!command.compare("get_book"))
        {
            tryBooksQuery(command, host, socket, payload, message, messageArr, responseArr, loginCookies, accessToken);
        }
        else if (!command.compare("add_book"))
        {
            tryAddBook(host, socket, payload, message, messageArr, responseArr, loginCookies, accessToken);
        }
        else if (!command.compare("delete_book"))
        {
            tryBooksQuery(command, host, socket, payload, message, messageArr, responseArr, loginCookies, accessToken);
        }
        else if (!command.compare("exit"))
        {
            close(socket);
            running = false;
        }

        // Since the connection is resetted at every iteration, we can close current socket
        close(socket);
    }

    return 0;
}
