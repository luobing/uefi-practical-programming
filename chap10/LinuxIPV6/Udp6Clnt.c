/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
//The Linux sample code of UDP6, luobing 20200408 13:00
//gcc -Wall UdpClnt.c -o UdpClnt
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define MAXBUF 1024
int main(int argc, char **argv)
{
    int sockfd, len;
    /* struct sockaddr_in dest; */ // IPv4
    struct sockaddr_in6 dest;      // IPv6
    char buffer[MAXBUF + 1];
 
    if (argc != 3) {
		printf("UDP6 Client.Usage: %s <IP> <port>", argv[0]);
		exit(1);
	}
	else
	{
		printf("Ready to start UDP6 Client. IP=%s,port=%d\n", argv[1], atoi(argv[2]));
	}

    //create socket
    /* if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { */ // IPv4
    if ((sockfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {      // IPv6
        perror("Socket()");
        exit(errno);
    }
    // printf("socket created\n");
 
    bzero(&dest, sizeof(dest));
    /* dest.sin_family = AF_INET; */  // IPv4
    dest.sin6_family = AF_INET6;     // IPv6
    /* dest.sin_port = htons(atoi(argv[2])); */ // IPv4
    dest.sin6_port = htons(atoi(argv[2]));     // IPv6
    /* if (inet_aton(argv[1], (struct in_addr *) &dest.sin_addr.s_addr) == 0) { */ // IPv4
    if ( inet_pton(AF_INET6, argv[1], &dest.sin6_addr) < 0 ) {                 // IPv6
        perror(argv[1]);
        exit(errno);
    }
    // printf("address created\n");
 
    /* Conect the server */
    if (connect(sockfd, (struct sockaddr *) &dest, sizeof(dest)) != 0) {
        perror("Connect() ");
        exit(errno);
    }
    // printf("server connected\n");
    
    while(1)
    {
        fputs("Input message(Q to quit):", stdout);
		fgets(buffer, MAXBUF, stdin);
        if (!strcmp(buffer, "q\n") || !strcmp(buffer, "Q\n"))
			break;

        len = send(sockfd, buffer, strlen(buffer), 0);
        if (len < 0)
        {
            printf("Send message to server fail! error code=%d: %s\n",errno,strerror(errno));
            break;
        }
        bzero(buffer,MAXBUF+1);
    
        len = recv(sockfd, buffer, MAXBUF, 0);
        buffer[len]='\0';
        printf("Message from server:%s", buffer);
    }

    close(sockfd);
    return 0;
}
