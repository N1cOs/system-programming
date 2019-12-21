#define PROTOCOL_VERSION "dirs/1.0"
#define BASE_HEADER "Base:"
#define STATUS_HEADER "Status:"

#define SEP "\0/"
#define SEP_LEN 2

#define FILE_SEP "\0\0"
#define FILE_SEP_LEN 2

#define END_NULLS 3
#define END_OF_STREAM "\0\0\0"
#define IS_END_OF_STREAM(nulls) nulls == END_NULLS

#define STATUS_OK '0'
#define STATUS_BAD '1'
#define STATUS_LEN 1

#define BAD_REQUEST "Bad request"
#define INTERNAL_ERROR "Internal server error"