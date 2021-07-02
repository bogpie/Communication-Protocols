#ifndef _QUERIES_
#define _QUERIES_

#include "helpers.h"
#include "warnings.h"
#include "response_parsers.h"
#include "requests.h"
#include "user_interaction.h"

// Try registering and print a warning if user is already logged in
void tryRegister(std::string host, int socket, nlohmann::json payload, std::string message,
                 char *messageArr, char *responseArr, std::string loginCookies);

// Try logging in and print a warning if user is already logged in; provides login cookies
void tryLogin(std::string host, int socket, nlohmann::json payload, std::string message,
              char *messageArr, char *responseArr, std::string &loginCookies);

// Try logging out and print a warning if user is not logged in. Updates login cookies and access token.
void tryLogout(std::string host, int socket, nlohmann::json payload, std::string message,
               char *messageArr, char *responseArr, std::string &loginCookies, std::string &accessToken);

// Try getting access to library. Updates access token
void tryEnterLibrary(std::string host, int socket, nlohmann::json payload, std::string message,
                     char *messageArr, char *responseArr, std::string loginCookies, std::string &accessToken);

// Try getting or deleting books based on provided command.
void tryBooksQuery(std::string command, std::string host, int socket, nlohmann::json payload, std::string message, char *messageArr,
                   char *responseArr, std::string loginCookies, std::string accessToken);

// Try adding book.
void tryAddBook(std::string host, int socket, nlohmann::json payload, std::string message, char *messageArr, char *responseArr,
                std::string loginCookies, std::string accessToken);

#endif