
#include "client_functions.h"

int printOptions(int);
void interface(int);
int get_auth_details(int, int);
void user_interface(int);
void admin_interface();

int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv;
    char *hello = "HI from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv, sizeof(serv)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    interface(sock);
    return 0;
}

