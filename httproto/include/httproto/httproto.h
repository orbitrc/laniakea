#ifndef _HTTPROTO_H
#define _HTTPROTO_H

#include <stdlib.h>
#include <stdint.h>
#ifndef cplusplus
    #include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


enum httproto_type {
    HTTPROTO_REQUEST,
    HTTPROTO_RESPONSE,
};

enum httproto_request_method {
    HTTPROTO_GET,
    HTTPROTO_POST,
    HTTPROTO_DELETE,
};

enum httproto_status_code {
    HTTPROTO_OK,
    HTTPROTO_NOT_FOUND,
};

typedef char* httproto_key_value_pair[2];
struct httproto_string_dictionary {
    httproto_key_value_pair *items;
    size_t length;
    size_t capacity;
};
typedef struct httproto_string_dictionary httproto_string_dictionary;

httproto_string_dictionary* httproto_string_dictionary_create();

void httproto_string_dictionary_set(httproto_string_dictionary *dict,
    const char *key, const char *val);

const char* httproto_string_dictionary_get(
    const httproto_string_dictionary *dict, const char *key);

void httproto_string_dictionary_free(httproto_string_dictionary *dict);


struct httproto_protocol {
    enum httproto_type type;
    enum httproto_request_method method;
    enum httproto_status_code status_code;
    char *uri;
    const char *query_string;
    httproto_string_dictionary *headers;
    uint64_t content_length;
    char *content;
    bool valid;
};
typedef struct httproto_protocol httproto_protocol;

/**
 * @brief   Create and initialize httproto_protocol and return new allocated
 *          pointer.
 * @param   type
 *          enum httproto_type (HTTPROTO_REQUEST or HTTPROTO_RESPONSE).
 * @return  New alloated httproto_protocol struct. This should be freed using
 *          httproto_protocol_free() manually.
 */
httproto_protocol* httproto_protocol_create(enum httproto_type type);


/**
 * @brief   httproto_protocol_parse
 * @param   protocol
 * @param   data
 * @param   len
 */
void httproto_protocol_parse(httproto_protocol *protocol, const char *data, size_t len);
void httproto_protocol_set_uri(httproto_protocol *protocol, const char *uri);
const char* httproto_protocol_get_uri(const httproto_protocol *protocol);
const char* httproto_protocol_get_path(const httproto_protocol *protocol);
const char* httproto_protocol_get_query_string(const httproto_protocol *protocol);
void httproto_protocol_set_header(httproto_protocol *protocol, const char *key, const char *val);
const char* httproto_protocol_get_header(const httproto_protocol *protocol, const char *key);
void httproto_protocol_free(httproto_protocol *protocol);


#ifdef __cplusplus
}
#endif

#endif /* _HTTPROTO_H */
