#include "user_interaction.h"

using namespace std;

void getCredentials(nlohmann::json &payload)
{
    string username, password;
    cout << "username=";
    getline(cin, username);

    cout << "password=";
    getline(cin, password);

    payload["username"] = username;
    payload["password"] = password;
}

void getBookInfo(nlohmann::json &payload)
{
    string title, author, genre, publisher, pageCountString;
    int pageCount;

    cout << "title=";
    getline(cin, title);

    cout << "author=";
    getline(cin, author);

    cout << "genre=";
    getline(cin, genre);

    cout << "publisher=";
    getline(cin, publisher);

    cout << "page count (enter a number) =";
    getline(cin, pageCountString);
    pageCount = stoi(pageCountString);

    payload["title"] = title;
    payload["author"] = author;
    payload["genre"] = genre;
    payload["publisher"] = publisher;
    payload["page_count"] = pageCount;

    return;
}