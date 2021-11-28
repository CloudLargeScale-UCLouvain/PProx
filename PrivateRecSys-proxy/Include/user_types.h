#ifndef USER_TYPES_H
#define USER_TYPES_H

#include <stdbool.h>

// Buffer size for incoming messages
const int MSG_LENGTH = 4096;

const char QUERIES_URI[] = "/engines/sgx_ur/queries";
const char EVENTS_URI[] = "/engines/sgx_ur/events";
const char QUERIES_USERID[] = "user";
const char QUERIES_KEYID[] = "key";
const char QUERIES_ITEMID[] = "item";
const char EVENTS_USERID[] = "entityId";
const char EVENTS_MOVIEID[] = "targetEntityId";
const char RESULTS[] = "result";

const int USER_KEY_LEN = 16;
const int IV_LEN = 8;

#endif
