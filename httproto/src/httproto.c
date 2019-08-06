#include <httproto/httproto.h>

#include <string.h>

#include <http_parser.h>

#ifdef __cplusplus
extern "C" {
#endif

httproto_string_dictionary* httproto_string_dictionary_create()
{
    httproto_string_dictionary *dict = (httproto_string_dictionary*)(malloc(
        sizeof(httproto_string_dictionary)));
    dict->length = 0;
    dict->capacity = 8;
    dict->items = (httproto_key_value_pair*)(calloc(dict->capacity,
        sizeof(httproto_key_value_pair)));
    for (size_t i = 0; i < dict->capacity; ++i) {
        dict->items[i][0] = NULL;
        dict->items[i][1] = NULL;
    }

    return dict;
}


void httproto_string_dictionary_set(httproto_string_dictionary *dict,
    const char *key, const char *val)
{
    size_t idx = dict->length;

    dict->items[idx][0] = (char*)malloc(strlen(key) + 1);
    strcpy(dict->items[idx][0], key);
    dict->items[idx][1] = (char*)malloc(strlen(val) + 1);
    strcpy(dict->items[idx][1], val);

    dict->length += 1;
}


const char* httproto_string_dictionary_get(
    const httproto_string_dictionary *dict,
    const char *key)
{
    for (size_t i = 0; i < dict->length; ++i) {
        if (strcmp(dict->items[i][0], key) == 0) {
            return dict->items[i][1];
        }
    }
    return NULL;
}


void httproto_string_dictionary_free(httproto_string_dictionary *dict)
{
    for (size_t i = 0; i < dict->length; ++i) {
        free(dict->items[i][0]);
        free(dict->items[i][1]);
    }
    free(dict->items);
    free(dict);
}


int parser_on_url(http_parser *parser, const char *at, size_t len);
int parser_on_header_field(http_parser *parser, const char *at, size_t len);
int parser_on_header_value(http_parser *parser, const char *at, size_t len);
int parser_on_body(http_parser *parser, const char *at, size_t len);


struct httproto_headers {
    char **header;
    size_t len;
};

struct parser_data {
    httproto_protocol *protocol;
    const char *header_key;
    size_t header_key_len;
};


httproto_protocol* httproto_protocol_create(enum httproto_type type)
{
    httproto_protocol *protocol = (httproto_protocol*)malloc(sizeof(httproto_protocol));
    protocol->type = type;

    protocol->headers = httproto_string_dictionary_create();

    return protocol;
}


void httproto_protocol_parse(httproto_protocol *protocol, const char *data, size_t len)
{
    http_parser parser;
    http_parser_settings settings;
    struct parser_data p_data;

    http_parser_init(&parser, (protocol->type == HTTPROTO_REQUEST) ? HTTP_REQUEST : HTTP_RESPONSE);
    http_parser_settings_init(&settings);
    settings.on_url = parser_on_url;
    settings.on_header_field = parser_on_header_field;
    settings.on_header_value = parser_on_header_value;
    settings.on_body = parser_on_body;

    p_data.protocol = protocol;
    p_data.header_key = NULL;
    p_data.header_key_len = 0;
    parser.data = (void*)(&p_data);
    http_parser_execute(&parser, &settings, data, len);

    switch (parser.method) {
    case HTTP_GET:
        protocol->method = HTTPROTO_GET;
        break;
    case HTTP_POST:
        protocol->method = HTTPROTO_POST;
        break;
    case HTTP_PUT:
        protocol->method = HTTPROTO_PUT;
        break;
    case HTTP_DELETE:
        protocol->method = HTTPROTO_DELETE;
        break;
    default:
        break;
    }

    // TODO: status code
}


void httproto_protocol_set_uri(httproto_protocol *protocol, const char *uri)
{
    if (protocol->uri != NULL) {
        free((void*)(protocol->uri));
        protocol->uri = NULL;
    }
    protocol->uri = (char*)calloc(strlen(uri) + 1, sizeof(char));
    protocol->query_string = protocol->uri; // FIXME
}


const char* httproto_protocol_get_uri(const httproto_protocol *protocol)
{
    return protocol->uri;
}


const char* httproto_protocol_get_path(const httproto_protocol *protocol)
{
    return protocol->uri;
}


const char* httproto_protocol_get_query_string(const httproto_protocol *protocol)
{
    return protocol->query_string;
}


void httproto_protocol_set_header(httproto_protocol *protocol, const char *key, const char *val)
{
    httproto_string_dictionary_set(protocol->headers, key, val);
}


const char* httproto_protocol_get_header(const httproto_protocol *protocol, const char *key)
{
    return httproto_string_dictionary_get(protocol->headers, key);
}


void httproto_protocol_free(httproto_protocol *protocol)
{
    // De-allocate c strings.
    free((void*)(protocol->uri));

    httproto_string_dictionary_free(protocol->headers);

    // De-allocate content body.
    free((void*)(protocol->content));

    // De-allocate protocol self.
    if (protocol != NULL) {
        free(protocol);
    }
}


//===========================
// Callbacks
//===========================
int parser_on_url(http_parser *parser, const char *at, size_t len)
{
    httproto_protocol *protocol = ((struct parser_data*)parser->data)->protocol;

    protocol->uri = (char*)calloc(len + 1, sizeof(char));
    strncpy(protocol->uri, at, len);

    return 0;
}
int parser_on_header_field(http_parser *parser, const char *at, size_t len)
{
    struct parser_data *p_data = (struct parser_data*)parser->data;

    if (p_data->header_key != NULL) {
        return 1;
    }
    p_data->header_key = at;
    p_data->header_key_len = len;

    return 0;
}
int parser_on_header_value(http_parser *parser, const char *at, size_t len)
{
    struct parser_data *p_data = (struct parser_data*)parser->data;
    httproto_protocol *protocol = p_data->protocol;
    char key[64];
    char val[64];

    strncpy(key, p_data->header_key, p_data->header_key_len);
    key[p_data->header_key_len] = '\0';
    strncpy(val, at, len);
    key[len] = '\0';
    httproto_string_dictionary_set(protocol->headers, key, val);

    p_data->header_key = NULL;

    return 0;
}
int parser_on_body(http_parser *parser, const char *at, size_t len)
{
    httproto_protocol *protocol = ((struct parser_data*)parser->data)->protocol;

    protocol->content = (char*)calloc(len, sizeof(char));
    strncpy(protocol->content, at, len);
    return 0;
}


#ifdef __cplusplus
}
#endif
