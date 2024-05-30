//       BRNO UNIVERSITY OF TECHNOLOGY
// COMPUTER COMMUNICATIONS AND NETWORKS COURSE
//               PROJECT N.1
//--------------------------------------------
//          AUTHOR: MICHAL BELOVEC
//           LOGIN: XBELOV04


#ifdef __unix__

    #include <sys/socket.h>
    #include <netdb.h>
    #include <unistd.h>

#elif defined(_WIN32) || defined(WIN32)

    #include <winsock2.h>
    #include <windows.h>
    #include <ws2tcpip.h>

#endif

#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>

void sigHandler(int);

int mode = -1; //TCP -> 0, UDP -> 1
int sock;
char sigBuffer[256];
struct addrinfo hints, *res;

int main(int argc, char* argv[]){
    int h_arg_index = -1;
    int p_arg_index = -1;
    if(argc < 7 && strcmp(argv[1], "--help") != 0){
        fprintf(stderr, "ERROR: incorrect arguments! Try \"--help\"!\n");
        exit(EXIT_FAILURE);
    }
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i],"-h") == 0){//Found -h the next arg should be the host name or IP adress
            i++;
            if(i < argc){
                h_arg_index = i;
            }
            else{//Ran out of arguments
                fprintf(stderr, "ERROR: incorrect arguments! Try \"--help\"!\n");
                exit(EXIT_FAILURE);
            }
        }
        else if(strcmp(argv[i],"-p") == 0){//Found -p the next arg should be the port number
            i++;
            if(i < argc){
                p_arg_index = i;
            }
            else{//Ran out of arguments
                fprintf(stderr, "ERROR: incorrect arguments! Try \"--help\"!\n");
                exit(EXIT_FAILURE);
            }
        }
        else if(strcmp(argv[i],"-m") == 0){
            i++;
            if(i < argc){
                if(strcmp(argv[i], "tcp") == 0){
                    mode = 0;
                }
                else if(strcmp(argv[i], "udp") == 0){
                    mode = 1;
                }
                else{
                    fprintf(stderr, "ERROR: unsupported mode!\nSupported modes: \"tcp\"|\"udp\".\n");
                    exit(EXIT_FAILURE);
                }
            }
            else{//Ran out of arguments
                fprintf(stderr, "ERROR: incorrect arguments! Try \"--help\"!\n");
                exit(EXIT_FAILURE);
            }
        }
        else if(strcmp(argv[1], "--help") == 0){
            fprintf(stderr, "Correct usage is as follows: ./ipkcpc -h <host> -p <port> -m <mode>\nArguments might be in any order.\n");
            return 0;
        }
        else{//Found an argument that doesnt match any allowed arguments
            fprintf(stderr, "ERROR: incorrect arguments! Try \"--help\"!\n");
            exit(EXIT_FAILURE);
        }
    }

    if(h_arg_index == -1 || p_arg_index == -1 || mode == -1){ //If there are missing arguments
        fprintf(stderr, "ERROR: incorrect arguments! Try \"--help\"!\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, sigHandler); //On detection of SIGINT, calls the sigHandler function
                                //which ends the connection gracefully.
    if(mode == 0){

        //Filling the hints structure with data
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        //The result will be in the placed in 'res'
        getaddrinfo(argv[h_arg_index], argv[p_arg_index], &hints, &res);

        //Creating the socket
        if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
            fprintf(stderr,"ERROR: error while creating socket\n");
            exit(EXIT_FAILURE);
        }

        //Establishing the connection
        if(connect(sock, res->ai_addr, res->ai_addrlen) != 0){
            fprintf(stderr,"ERROR: error while connecting\n");
            exit(EXIT_FAILURE);
        }

        char buf[256];
        int sent;
        int recvd;

        while(1){

            //Clearing the buffer and reading a line from stdin into the buffer
            bzero(buf, 256);
            if(fgets(buf, 256, stdin) == NULL){
                fprintf(stderr,"ERROR: error fgets\n");
                close(sock);
                exit(EXIT_FAILURE);
            }

            //Sending the buffer with the message to the server
            if((sent = send(sock, buf, strlen(buf), 0)) < 0){
                fprintf(stderr,"ERROR: error while sending message\n");
                close(sock);
                exit(EXIT_FAILURE);
            }

            //Clearing the buffer before receiving the reply from the server
            bzero(buf, 256);
            //Wainting for the reply
            if((recvd = recv(sock, buf, 256, 0)) < 0){
                fprintf(stderr,"ERROR: error while recieving message\n");
                close(sock);
                exit(EXIT_FAILURE);
            }

            //If the reply is BYE, break from the while cycle and end the connection
            if(strcmp(buf, "BYE\n") == 0){
                printf("%s", buf);
                break;
            }
            else{
                printf("%s", buf);
            }
        }
        close(sock);
    }
    if(mode == 1){

        //Filling the hints structure with data
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;

        //The result will be in the placed in 'res'
        getaddrinfo(argv[h_arg_index], argv[p_arg_index], &hints, &res);

        //Creating datagram socket
        if((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
            fprintf(stderr,"ERROR: error while creating socket\n");
            exit(EXIT_FAILURE);
        }

        //Initialization of local variables used in the transmission
        char buf[512];
        char another_buf[512];
        int sent;
        int recvd;

        while(1){

            //Clearing the buffers before reading into them with fgets
            bzero(another_buf, 512);
            bzero(buf, 512);
            if(fgets(another_buf, 512, stdin) == NULL){
                fprintf(stderr,"ERROR: error fgets\n");
                close(sock);
                exit(EXIT_FAILURE);
            }

            //0th byte is the OPCODE where '\0' means request
            buf[0] = '\0';
            //1st byte is the length of our message
            //Casting to char because we have a char buffer
            buf[1] = (char) strlen(another_buf);
            //Copying the message from the aditional buffer to the buffer we will be sending
            for(int j = 0; j < strlen(another_buf); j++){
                buf[2+j] = another_buf[j];
            }

            //Send the buffer with our message
            if((sent = sendto(sock, buf, (strlen(another_buf)+2), 0, res->ai_addr, res->ai_addrlen)) == -1){
                fprintf(stderr,"ERROR: error sendto\n");
                close(sock);
                exit(EXIT_FAILURE);
            }

            //Clearing buffer before receiving
            bzero(buf, 512);
            if((recvd = recvfrom(sock, buf, 512, 0,res->ai_addr, &(res->ai_addrlen))) == -1){
                fprintf(stderr,"ERROR: error recvfrom\n");
                close(sock);
                exit(EXIT_FAILURE);
            }

            //Printing out the reply
            //1st index of the reply is the status code, 0 for OK, 1 for ERROR
            if(buf[1] == 0){
                printf("OK:%s\n", buf+3);
            }
            else{
                printf("ERR:%s\n", buf+3);
            }
        }
        return 0;
    }

    return 0;
}

//Funtion that handles the SIGINT signal by terminating the connection (only with TCP)
//and closing the socket
void sigHandler(int dummy){
    if(mode == 0){
        bzero(sigBuffer, 256);
        strcpy(sigBuffer, "BYE\n");
        if((send(sock, sigBuffer, strlen(sigBuffer), 0)) < 0){
            fprintf(stderr,"ERROR: error while sending message\n");
            exit(EXIT_FAILURE);
        }

        bzero(sigBuffer, 256);
        if((recv(sock, sigBuffer, 256, 0)) < 0){
            fprintf(stderr,"ERROR: error while recieving message\n");
            exit(EXIT_FAILURE);
        }
        printf("%s",sigBuffer);
        close(sock);
        exit(EXIT_SUCCESS);
    }
    else{
        close(sock);
        exit(EXIT_SUCCESS);
    }
}
