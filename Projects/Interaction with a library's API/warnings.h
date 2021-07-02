#ifndef _WARNINGS_
#define _WARNINGS_

#include "helpers.h"

// Print possible warning in case user is already logged in
void printLoggedIn(std::string loginCookies);

// Print possible warning in case user is already logged out
void printLoggedOut(std::string loginCookies);

// Print possible warning in case user hasn't obtained a library access token
void printNoAccess(std::string accessToken);

#endif