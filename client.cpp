#include <iostream>
#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.hpp"
#include "requests.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    int PORT = 8080;
    char* HOST = (char*)"34.118.48.238";
    int sockfd;
    char command[BUFLEN];
    char username[100], password[100];
    char payload[2000];
    //Variabile folosit pentru a retine cookie-ul atunci cand un user
    //se logeaza, respectiv pentru a retine tokenul JWT atunci cand se primeste
    //acces la librarie.
    char **cookies, *tokenJWT;
    cookies = (char**) malloc(100 * sizeof(char*));

    while(1) {
        //Se citesc comenzi de la tastatura si se deschide conexiunea cu serverul.
        memset(command, 0, BUFLEN);
        fgets(command, BUFLEN - 1, stdin);
        sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

        if (strcmp(command, "register\n") == 0) {
            cout << "---------------REGISTER----------------\n";
            cout << "username="; cin >> username;
            cout << "password="; cin >> password;
            sprintf(payload,"{\"username\":\"%s\",\"password\":\"%s\"}", username, password);

            //Completam structura, iar apoi trimitem mesajul catre server.
            post_request req;
            req.host = strdup(HOST);
            req.url = strdup("/api/v1/tema/auth/register");
            req.content_type = strdup("application/json");
            req.body_data = (char**) malloc(sizeof(char*));
            req.body_data[0] = strdup(payload);
            req.body_data_fields_count = 1;

            char *request, *response;
            //Apelam functia compute_post_request fara niciun cookies.
            request = compute_post_request(req, NULL, 0);
            send_to_server(sockfd, request);
            //Primim raspunul de la serval si il parsam in functie de codul
            //acestuia.
            //Facem o copie a raspunsului si gasim codul care este al doilea cuvant
            //din raspuns.
            response = receive_from_server(sockfd);
            char *aux = strdup(response);
            char *error = strtok(strstr(aux, " "), " ");

            if (error[0] == '2') {
                cout << "Register successfully!\n";
            } else {
                //Extragem doar mesajul de eroare din json.
                char *message = basic_extract_json_response(response);
                message[strlen(message) - 2] = '\0';
                cout << message + 10 << endl;
            }
        }

        //Procedam asemanator ca la register.
        if (strcmp(command, "login\n") == 0) {
            free(cookies[0]);
            tokenJWT = NULL;

            cout << "---------------LOGIN----------------\n";
            cout << "username="; cin >> username;
            cout << "password="; cin >> password;
            sprintf(payload,"{\"username\":\"%s\",\"password\":\"%s\"}", username, password);

            post_request req;
            req.host = strdup(HOST);
            req.url = strdup("/api/v1/tema/auth/login");
            req.content_type = strdup("application/json");
            req.body_data = (char**) malloc(sizeof(char*));
            req.body_data[0] = strdup(payload);
            req.body_data_fields_count = 1;

            char *request, *response;
            //Apelam functia compute_post_request fara niciun cookies.
            request = compute_post_request(req, NULL, 0);
            send_to_server(sockfd, request);
            response = receive_from_server(sockfd);
            char *aux = strdup(response);
            char *error = strtok(strstr(aux, " "), " ");
           
            if (error[0] == '2') {
                cout << "Login successfully!\n";
                //Pastram salvat cookie-ul parsat din raspunsul de la server.
                cookies[0] = strdup(extract_cookie(response));
            } else {
                char *message = basic_extract_json_response(response);
                message[strlen(message) - 2] = '\0';
                cout << message + 10 << endl;
            }
            
        }

        //Procedam asemanator ca la register.
        if (strcmp(command, "enter_library\n") == 0) {
            get_request req;
            req.host = strdup(HOST);
            req.url = strdup("/api/v1/tema/library/access");
            req.query_params = NULL;

            char *request, *response;
            //Apelam functia compute_get_request cu valoarea lui cookie
            //care este NULL daca nu s-a conectat userul sau cookie-ul
            //salvat la login.
            request = compute_get_request(req, cookies, 1);
            send_to_server(sockfd, request);
            response = receive_from_server(sockfd);
            char *aux = strdup(response);
            char *error = strtok(strstr(aux, " "), " ");
           
            if (error[0] == '2') {
                //Pastram salvat tokenul parsat din raspunsul de la server.
                tokenJWT = extract_jwt(response);
                cout << "-------------ENTER LIBRARY--------------\n";
                cout << "Access to library!\n";
            } else {
                char *message = basic_extract_json_response(response);
                message[strlen(message) - 2] = '\0';
                cout << message + 10 << endl;
            }
        }

        //Procedam asemanator ca la register.
        if (strcmp(command, "get_books\n") == 0) {
            cout << "---------------GETS BOOK----------------\n";
            get_request jwtReq;
            jwtReq.host = strdup(HOST);
            jwtReq.url = strdup("/api/v1/tema/library/books");
            jwtReq.query_params = NULL;

            char *request, *response;
            //Apelam functia compute_get_requestJWT cu valoarea lui tokenJWT
            //care este NULL daca nu s-a obtinut accesul la biblioteca sau
            //valoarea obtinuta la enter_library.
            request = compute_get_requestJWT(jwtReq, tokenJWT);
            send_to_server(sockfd, request);
            response = receive_from_server(sockfd);
            char *aux = strdup(response);
            char *error = strtok(strstr(aux, " "), " ");

            if (error[0] == '2') {
                if(basic_extract_json_response(response)) {
                    cout << "[" << basic_extract_json_response(response) << endl;
                } else {
                    cout << "[]\n";
                }
            } else {
                char *message = basic_extract_json_response(response);
                message[strlen(message) - 2] = '\0';
                cout << message + 10 << endl;
            }
        }

        
        if (strcmp(command, "get_book\n") == 0) {
            cout << "---------------GET BOOK----------------\n";

            // Oferim prompt pentru id si construim url-ul.
            char id[50];
            cout << "id="; cin >> id;
            char url[500];
            strcpy(url, "/api/v1/tema/library/books/");
            strcat(url, id);

            get_request req;
            req.host = strdup(HOST);
            req.url = strdup(url);
            req.query_params = NULL;

            char *request, *response;
            request = compute_get_requestJWT(req, tokenJWT);
            send_to_server(sockfd, request);
            response = receive_from_server(sockfd);
            char *aux = strdup(response);
            char *error = strtok(strstr(aux, " "), " ");

            if (error[0] == '2') {
                cout << "[" << basic_extract_json_response(response) << endl;
            } else {
                char *message = basic_extract_json_response(response);
                message[strlen(message) - 2] = '\0';
                cout << message + 10 << endl;
            }
        }

        if(strcmp(command, "add_book\n") == 0) {
            cout << "---------------ADD BOOK----------------\n";
            char title[50], author[50], genre[50], publisher[50];
            int page_count;
            //Citim datele cartii pe care vrem sa o introducem.
            cout << "title="; cin >> title;
            cout << "author="; cin >> author;
            cout << "genre="; cin >> genre;
            cout << "publisher="; cin >> publisher;
            cout << "page_count="; cin >> page_count;

            char payload[1000];
            sprintf(payload,"{\"title\":\"%s\",\"author\":\"%s\",\"genre\":\"%s\",\"page_count\":%d,\"publisher\":\"%s\"}", 
                title, author, genre, page_count, publisher);

            post_request req;
            req.host = strdup(HOST);
            req.url = strdup("/api/v1/tema/library/books");
            req.content_type = strdup("application/json");
            req.body_data = (char**) malloc(sizeof(char*));
            req.body_data[0] = strdup(payload);
            req.body_data_fields_count = 1;

            char *request, *response;
            //Apelam functia compute_jwt_request cu valoarea lui tokenJWT
            //(descrisa la get_books) si type-ul "POST".
            request = compute_jwt_request(req, tokenJWT, (char*)"POST");
            send_to_server(sockfd, request);
            response = receive_from_server(sockfd);
            char *aux = strdup(response);
            char *error = strtok(strstr(aux, " "), " ");

            if (error[0] == '2') {
                cout << "All good!\n";
            } else {
                char *message = basic_extract_json_response(response);
                message[strlen(message) - 2] = '\0';
                cout << message + 10 << endl;
            }
        }

        if(strcmp(command, "delete_book\n") == 0) {
            cout << "-------------DELETE BOOK--------------\n";

            char id[50];
            cout << "id="; cin >> id;
            char url[500];
            strcpy(url, "/api/v1/tema/library/books/");
            strcat(url, id);

            post_request req;
            req.host = strdup(HOST);
            req.url = strdup(url);
            req.content_type = strdup("application/json");
            req.body_data = NULL;
            req.body_data_fields_count = 0;

            char *request, *response;
            //Asemanator cu add_book, doar ac typeul este "DELETE".
            request = compute_jwt_request(req, tokenJWT, (char*)"DELETE");
            send_to_server(sockfd, request);
            response = receive_from_server(sockfd);
            char *aux = strdup(response);
            char *error = strtok(strstr(aux, " "), " ");

            if (error[0] == '2') {
                cout << "All good!\n";
            } else {
                char *message = basic_extract_json_response(response);
                message[strlen(message) - 2] = '\0';
                cout << message + 10 << endl;
            }
        }

        if(strcmp(command, "logout\n") == 0) {
            cout << "---------------LOGOUT----------------\n";
            get_request getReq;
            getReq.host = strdup(HOST);
            getReq.url = strdup("/api/v1/tema/auth/logout");
            getReq.query_params = NULL;

            char *request, *response;
            //Asemanator cu enter_library
            request = compute_get_request(getReq, cookies, 1);
            send_to_server(sockfd, request);
            response = receive_from_server(sockfd);
            char *aux = strdup(response);
            char *error = strtok(strstr(aux, " "), " ");
           
            if (error[0] == '2') {
                cout << "Thank you!" << endl;
                cookies[0] = NULL;
                tokenJWT = NULL;
            } else {
                char *message = basic_extract_json_response(response);
                message[strlen(message) - 2] = '\0';
                cout << message + 10 << endl;
            }
        }

        if(strcmp(command, "exit\n") == 0) {
            close_connection(sockfd);
            break;
        }

        close_connection(sockfd);
    }
    return 0;
}
