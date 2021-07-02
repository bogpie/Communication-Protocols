#include "warnings.h"

using namespace std;

void printLoggedIn(string loginCookies)
{
    if (loginCookies.empty())
    {
        return;
    }
    else
    {
        cout << "Warning! You logged in previously!\n";
        return;
    }
}

void printLoggedOut(string loginCookies)
{
    if (!loginCookies.empty())
    {
        return;
    }
    else
    {
        cout << "Warning! No previous login detected!\n";
        return;
    }
}


void printNoAccess(string accessToken)
{
    if (accessToken.empty())
    {
        cout << "Warning! No access token provided!\n";
        return;
    }
    return;
}