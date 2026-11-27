#if defined _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #define closesocket close
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

#include <stdio.h>
#include <string.h>
#define QUEUE_LENGHT 6
#define BUFFER_SIZE 1024

int main() {

    #if defined _WIN32
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (result != 0) {
            printf("Errore di inizializzazione della socket.\n");
            return -1;
        }
    #endif
    
    int server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket < 0) {
        printf("Creazione della socket fallita.\n");
        #if defined _WIN32
            WSACleanup();
        #endif
        return -2;
    }


    int server_port = -1;

        #if defined _WIN32
            do{
                printf("Inserisci un numero di porta per il server (0 - 65535): ");
                scanf("%d", &server_port);
                if(server_port < 0 || server_port > 65535) {
                    printf("Valore per la porta non valido.\n");
                }
            } while (server_port < 0 || server_port > 65535);   
        #else
            do{
                printf("Inserisci un numero di porta per il server (1024 - 65535): ");
                scanf("%d", &server_port);
                if(server_port < 1024 || server_port > 65535) {
                    printf("Valore per la porta non valido.\n");
                }
            } while (server_port < 1024 || server_port > 65535);
        #endif
    

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons((unsigned short)server_port);

    // inet_pton converte IP in binario per checkare sia corretto 
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        printf("Indirizzo IP del server non valido.\n");
        closesocket(server_socket);
        #if defined _WIN32
            WSACleanup();
        #endif
        return -3;
    }

    server_address.sin_port = htons(server_port);


    if (bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) != 0) {
        printf("Errore di binding della socket server.\n");
        closesocket(server_socket);
        #if defined _WIN32
            WSACleanup();
        #endif
        return -4;
    }

    if (listen(server_socket, QUEUE_LENGHT) != 0) {
        printf("Errore di inizializzazione dell'ascolto per il socket server.\n");
        closesocket(server_socket);
        #if defined _WIN32
            WSACleanup();
        #endif
        return -5;
    }

    struct sockaddr_in client_address;
    int client_socket;
    #if defined _WIN32
        int client_len;
    #else
        socklen_t client_len;
    #endif

    printf("In attesa di connessione con un client...\n");

    while (1) {

        client_len = sizeof(client_address);
            

        client_socket = accept(server_socket, (struct sockaddr*) &client_address, &client_len);
        if (client_socket < 0) {
            printf("Errore di inizializzazione del socket client.\n");
            closesocket(server_socket);
            #if defined _WIN32
                WSACleanup();
            #endif
            return -6;
        }

        printf("Connessione effettuata al client: %s\n", inet_ntoa(client_address.sin_addr));

        char connection_successful_string[] = "Connessione effettuata correttamente.\n";
        int con_string_len = strlen(connection_successful_string);

        if (send(client_socket, connection_successful_string, con_string_len, 0) != con_string_len) {
            printf("Errore di comunicazione con il client.\n");
            closesocket(client_socket);
            continue;
        }

        int numeri[3];
        int bytes_received;
        int res;
        char out_msg[32];
        int out_msg_len;

        do {

            bytes_received = recv(client_socket, (char*) &numeri, sizeof(numeri), 0);

            if (bytes_received <= 0) {
                printf("Client disconnesso o errore di ricezione.\n");
                break;
            }

            res = numeri[0] + numeri[1] - numeri[2];
            printf("%d + %d - %d = %d\n", numeri[0], numeri[1], numeri[2], res); 

        
            if (res == 100) {
                snprintf(out_msg, sizeof(out_msg), "QUIT");
                printf("Client disconnesso\n");
            } else {
                snprintf(out_msg, sizeof(out_msg), "%d", res);
            }

            out_msg_len = strlen(out_msg);
            if (send(client_socket, out_msg, out_msg_len, 0) != out_msg_len) {
                printf("Errore di comunicazione con il client.\n");
                break;
            }

        } while (strcmp(out_msg, "QUIT") != 0);
        
        closesocket(client_socket);
        #if defined _WIN32
            WSACleanup();
        #endif
    }

    closesocket(server_socket);

    #if defined _WIN32
        WSACleanup();
    #endif

    return 0;
}