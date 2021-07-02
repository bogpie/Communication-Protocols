#ifndef _USERINTERACTION_
#define _USERINTERACTION_

#include "helpers.h"

// Ask user for credentials and fill payload
void getCredentials(nlohmann::json &payload);

// Ask user for added book data and fill payload 
void getBookInfo(nlohmann::json &payload);

#endif