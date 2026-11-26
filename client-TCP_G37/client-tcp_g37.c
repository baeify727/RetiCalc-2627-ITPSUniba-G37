#if defined WIN32
    #include <winsock.h>
#else
    #define closesocket close
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

#include <stdio.h>
#include <string.h>

int main() {

    #if defined WIN32
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (result != 0) {
            printf("Errore di inizializzazione della socket.\n");
            return -1;
        }
    #endif

    int client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket < 0) {
        printf("Creazione della socket fallita.\n");
        #if defined WIN32
            WSACleanup();
        #endif
        return -2;
    }

    char server_ip[16];
    int server_port;

    printf("Inserisci l'IP del server: ");
    scanf("%15s", server_ip);
    printf("Inserisci la porta del server: ");
    scanf("%d", &server_port);

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(server_ip);
    server_address.sin_port = htons(server_port);


    if (connect(client_socket, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
        printf("Connessione al server fallita.\n");
        #if defined WIN32
            WSACleanup();
        #endif
        return -3;
    }
    printf("Connessione al server effettuata correttamente.\n");



    closesocket(client_socket);

    #if defined WIN32
        WSACleanup();
    #endif

    return 0;
}