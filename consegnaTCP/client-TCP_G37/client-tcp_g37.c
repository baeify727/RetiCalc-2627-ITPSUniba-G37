#if defined _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef SOCKET socket_t;
    #define wait() Sleep(2000)
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define closesocket close
    #define wait() sleep(2)
    #define INVALID_SOCKET (-1)
    typedef int socket_t;
#endif

#include <stdio.h>
#include <string.h>
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

    socket_t client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket == INVALID_SOCKET) {
        printf("Creazione della socket fallita.\n");
        #if defined _WIN32
            wait();
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
    server_address.sin_port = htons((unsigned short)server_port);

    if (inet_pton(AF_INET, server_ip, &server_address.sin_addr) <= 0) {
        printf("Indirizzo IP del server non valido.\n");
        closesocket(client_socket);
        #if defined _WIN32
            wait();
            WSACleanup();
        #endif
        return -3;
    }

    if (connect(client_socket, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
        printf("Connessione al server fallita.\n");
        closesocket(client_socket);
        #if defined _WIN32  
            wait();
            WSACleanup();
        #endif
        return -4;
    }

    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);

    if (bytes_received < 0) {
        printf("Errore nella ricezione dei dati dal server.\n");
        closesocket(client_socket);
        #if defined _WIN32
            wait();
            WSACleanup();
        #endif
        return -5;
    } else if (bytes_received == 0) {
        printf("Il server ha chiuso la connessione.\n");
        closesocket(client_socket);
        #if defined _WIN32
            wait();
            WSACleanup();
        #endif
        return -6;
    }

    buffer[bytes_received] = '\0';
    printf("%s", buffer);
    memset(&buffer, 0, sizeof(buffer));

    char res[32];
    int numeri[3];
    int ch;

    do {

        memset(&res, 0, sizeof(res));
        bytes_received = 0;

        printf("Inserisci 3 numeri, separati da uno spazio: ");

        while(scanf("%d %d %d", &numeri[0], &numeri[1], &numeri[2]) != 3) {
            printf("Input non valido, riprova: ");
            while ((ch = getchar()) != '\n' && ch != EOF);
        }

        if (send(client_socket, (char*)numeri, sizeof(numeri), 0) != sizeof(numeri)) {
            printf("Errore di comunicazione con il server.\n");
            closesocket(client_socket);
            #if defined _WIN32
                wait();
                WSACleanup();
            #endif
            return -7;
        }

    
        bytes_received = recv(client_socket, res, sizeof(res)-1, 0);

        if (bytes_received < 0) {
            printf("Errore nella ricezione dei dati dal server.\n");
            closesocket(client_socket);
            #if defined _WIN32
                wait();
                WSACleanup();
            #endif
            return -5;
        } else if (bytes_received == 0) {
            printf("Il server ha chiuso la connessione.\n");
            closesocket(client_socket);
            #if defined _WIN32
                wait();
                WSACleanup();
            #endif
            return -6;
        }

        res[bytes_received] = '\0';
        printf("Risultato: %s\n", res);

    } while (strcmp(res, "QUIT") != 0);


    closesocket(client_socket);
    #if defined _WIN32
        wait();
        WSACleanup();
    #endif
    return 0;
}