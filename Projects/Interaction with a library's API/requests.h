#ifndef _REQUESTS_
#define _REQUESTS_

#include "helpers.h"

#define REGISTER_URL "/api/v1/tema/auth/register"
#define LOGIN_URL "/api/v1/tema/auth/login"
#define LOGOUT_URL "/api/v1/tema/auth/logout"
#define ENTER_LIBRARY_URL "/api/v1/tema/library/access"
#define BOOKS_URL "/api/v1/tema/library/books"

#define JSON_TYPE "application/json"

// computes and returns a POST request string (cookies can be NULL if not needed)
std::string computePostRequest(std::string host, std::string url, std::string contentType, std::string payload,
							   std::string loginCookie, std::string accessToken);

// computes and returns a GET request string
std::string computeGetDeleteRequest(std::string method, std::string host, std::string url, std::string loginCookie,
									std::string accessToken);

#endif
