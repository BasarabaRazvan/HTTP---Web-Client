#ifndef _REQUESTS_
#define _REQUESTS_

// computes and returns a GET request string (query_params
// and cookies can be set to NULL if not needed)
char *compute_get_request(get_request request, char **cookies, int cookies_count);

char *compute_get_requestJWT(get_request request, char *tokemJWT);

// computes and returns a POST request string (cookies can be NULL if not needed)
char *compute_post_request(post_request request, char **cookies, int cookies_count);

// computes and returns a DELETE request string
char *compute_jwt_request(post_request request, char *tokemJWT, char *type);

#endif
