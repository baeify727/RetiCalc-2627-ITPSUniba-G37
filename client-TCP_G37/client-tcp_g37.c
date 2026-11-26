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
#define BUFFER_SIZE 1024

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
        closesocket(client_socket);
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

    if (server_address.sin_addr.s_addr == INADDR_NONE) {
        printf("Indirizzo IP del server non valido.\n");
        closesocket(client_socket);
        #if defined WIN32
            WSACleanup();
        #endif
    return -3;
    }


    if (connect(client_socket, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
        printf("Connessione al server fallita.\n");
        closesocket(client_socket);
        #if defined WIN32
            WSACleanup();
        #endif
        return -4;
    }

    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);

    if (bytes_received < 0) {
        printf("Errore nella ricezione dei dati dal server.\n");
        closesocket(client_socket);
        #if defined WIN32
            WSACleanup();
        #endif
        return -5;
    } else if (bytes_received == 0) {
        printf("Il server ha chiuso la connessione.\n");
        closesocket(client_socket);
        #if defined WIN32
            WSACleanup();
        #endif
        return -6;
    }

    buffer[bytes_received] = '\0';
    printf("%s", buffer);
    memset(&buffer, 0, sizeof(buffer));

    int numeri[3];
    while(scanf("%d %d %d", &numeri[0], &numeri[1], &numeri[2]) != 3) {
        printf("Input non valido, riprova: ");
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
    }

    if (send(client_socket, (char*)numeri, sizeof(numeri), 0) != sizeof(numeri)) {
            printf("Errore di comunicazione con il client.\n");
            closesocket(client_socket);
    }

    char res[32];
    int res_len = strlen(res);
    bytes_received = recv(client_socket, res, res_len, 0);

    if (bytes_received < 0) {
        printf("Errore nella ricezione dei dati dal server.\n");
        closesocket(client_socket);
        #if defined WIN32
            WSACleanup();
        #endif
        return -5;
    } else if (bytes_received == 0) {
        printf("Il server ha chiuso la connessione.\n");
        closesocket(client_socket);
        #if defined WIN32
            WSACleanup();
        #endif
        return -6;
    }

    res[bytes_received] = '\0';
    if(strcmp(res, "QUIT") == 0) {
        printf("%d\n", *res);
    } else {
        printf("%s\n", res);
    }
    



    
    closesocket(client_socket);
    #if defined WIN32
        WSACleanup();
    #endif

    return 0;
}