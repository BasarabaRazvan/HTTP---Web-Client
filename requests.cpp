#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.hpp"
#include "requests.hpp"

char *compute_get_request(get_request request, char **cookies, int cookies_count) {
    char *message = (char*) calloc(BUFLEN, sizeof(char));
    char *line = (char*) calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (request.query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", request.url, request.query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", request.url);
    }

    compute_message(message, line);

    // Step 2: add the host

    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", request.host);

    compute_message(message, line);

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (cookies != NULL) {
        for (int i = 0; i < cookies_count; ++i) {
            memset(line, 0, LINELEN);
            sprintf(line, "Cookie: %s", cookies[i]);

            compute_message(message, line);
        }
    }
    // Step 4: add final new line
    compute_message(message, "");
    return message;
}


char *compute_get_requestJWT(get_request request, char *tokenJWT) {
    char *message = (char*) calloc(BUFLEN, sizeof(char));
    char *line = (char*) calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (request.query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", request.url, request.query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", request.url);
    }

    compute_message(message, line);

    // Step 2: add the host

    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", request.host);

    compute_message(message, line);

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (tokenJWT != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", tokenJWT);

        compute_message(message, line);
    }
    // Step 4: add final new line
    compute_message(message, "");
    return message;
}


char *compute_post_request(post_request request, char **cookies, int cookies_count)
{
    char *message = (char*) calloc(BUFLEN, sizeof(char));
    char *line = (char*) calloc(LINELEN, sizeof(char));
    char *body_data_buffer = (char*) calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", request.url);
    compute_message(message, line);
    
    // Step 2: add the host

    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", request.host);

    compute_message(message, line);

    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */
    strcpy(body_data_buffer, "");
    for (int i = 0; i < request.body_data_fields_count; i++) {
        strcat(body_data_buffer, request.body_data[i]);
        if (i != request.body_data_fields_count - 1) {
            strcat(body_data_buffer, "&");
        }
    }

    sprintf(line, "Content-Type: %s", request.content_type);
    compute_message(message, line);
    sprintf(line, "Content-Length: %ld", strlen(body_data_buffer));
    compute_message(message, line);

    // Step 4 (optional): add cookies
    if (cookies != NULL) {
       for (int i = 0; i < cookies_count; ++i) {
            memset(line, 0, LINELEN);
            sprintf(line, "Cookie: %s", cookies[i]);

            compute_message(message, line);
        }
    }
    // Step 5: add new line at end of header
    compute_message(message, "");

    // Step 6: add the actual payload data
    memset(line, 0, LINELEN);
    compute_message(message, body_data_buffer);

    free(line);
    return message;
}


char *compute_jwt_request(post_request request, char *tokenJWt, char *type)
{
    char *message = (char*) calloc(BUFLEN, sizeof(char));
    char *line = (char*) calloc(LINELEN, sizeof(char));
    char *body_data_buffer = (char*) calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "%s %s HTTP/1.1", type, request.url);
    compute_message(message, line);
    
    // Step 2: add the host

    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", request.host);

    compute_message(message, line);

    strcpy(body_data_buffer, "");
    for (int i = 0; i < request.body_data_fields_count; i++) {
        strcat(body_data_buffer, request.body_data[i]);
        if (i != request.body_data_fields_count - 1) {
            strcat(body_data_buffer, "&");
        }
    }

    sprintf(line, "Content-Type: %s", request.content_type);
    compute_message(message, line);
    sprintf(line, "Content-Length: %ld", strlen(body_data_buffer));
    compute_message(message, line);

    // Step 4 (optional): add cookies
    if (tokenJWt != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", tokenJWt);

        compute_message(message, line);
    }
    // Step 5: add new line at end of header
    compute_message(message, "");

    // Step 6: add the actual payload data
    memset(line, 0, LINELEN);
    compute_message(message, body_data_buffer);

    free(line);
    return message;
}
