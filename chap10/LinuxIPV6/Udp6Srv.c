/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
//The Linux sample code of UDP6, luobing 20200408 11:00
//gcc -Wall UdpSrv.c -o UdpSrv
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#define MAXBUF 1024
int main(int argc, char **argv)
{
    int sockfd;
    // socklen_t len;
    int strLen;
    /* struct sockaddr_in my_addr, their_addr; */ // IPv4
    struct sockaddr_in6 my_addr; // IPv6
    int my_addr_len;
 
    unsigned int SrvPort;
    char buf[MAXBUF + 1];
    char clientIdbuf[300];
    
    if (argc != 2) {
		printf("UDP6 Server. Usage: %s <port>", argv[0]);
		exit(1);
	}
	else
	{
		printf("Ready to start UDP6 Server. port=%d\n", atoi(argv[1]));
	}

    if (argv[1])
        SrvPort = atoi(argv[1]);
    else
        SrvPort = 8888;
    
 
    /* if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) { */ // IPv4
    if ((sockfd = socket(PF_INET6, SOCK_DGRAM, 0)) == -1) { // IPv6
        perror("socket()");
        exit(1);
    } else
        printf("socket() created\n");
 
    bzero(&my_addr, sizeof(my_addr));
    /* my_addr.sin_family = PF_INET; */ // IPv4
    my_addr.sin6_family = AF_INET6;    // IPv6
    /* my_addr.sin_port = htons(SrvPort); */ // IPv4
    my_addr.sin6_port = htons(SrvPort);   // IPv6
    if (argv[3])
        /* my_addr.sin_addr.s_addr = inet_addr(argv[3]); */ // IPv4
        inet_pton(AF_INET6, argv[3], &my_addr.sin6_addr);  // IPv6
    else
        /* my_addr.sin_addr.s_addr = INADDR_ANY; */ // IPv4
        my_addr.sin6_addr = in6addr_any;            // IPv6
 
    /* if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) */ // IPv4
    if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in6))  // IPv6
        == -1) {
        perror("bind()");
        exit(1);
    } else
        printf("bind() success!\n");
 
    // if (listen(sockfd, 10) == -1) {  //max 10 links
    //     perror("listen()");
    //     exit(1);
    // } else
    //     printf("Begin listen...\n");
    my_addr_len=sizeof(struct sockaddr_in6);
    while(1)
    {
        bzero(buf, MAXBUF + 1);
        bzero(clientIdbuf, 300);
		strLen = recvfrom(sockfd,buf,MAXBUF,0,(struct sockaddr *)&my_addr,(socklen_t*)&my_addr_len);
        if(strLen>0)
        {
            inet_ntop(AF_INET6,&my_addr.sin6_addr,clientIdbuf,sizeof(clientIdbuf));
            printf("Client ip %s\n",clientIdbuf);
            buf[strLen]='\0';
            printf("Msg from Client:%s",buf);
        }
        if(sendto(sockfd,buf,strLen,0,(struct sockaddr *)&my_addr,my_addr_len)<0)
        {
            printf("Error!\n");
            break;
        }

    }
    // for (i = 0; i < 10; i++) {
    //     len = sizeof(struct sockaddr);
    //     if ((new_fd = accept(sockfd, (struct sockaddr *) &their_addr,&len)) == -1) {
    //         perror("accept()");
    //         exit(errno);
    //     } else
    //         printf("Connected! clinet = %s, port=%d, socket=%d\n",
    //                /* inet_ntoa(their_addr.sin_addr), */ // IPv4
    //                inet_ntop(AF_INET6, &their_addr.sin6_addr, buf, sizeof(buf)), // IPv6
    //                /* ntohs(their_addr.sin_port), new_fd); */ // IPv4
    //                their_addr.sin6_port, new_fd); // IPv6
 
       
    //     bzero(buf, MAXBUF + 1);

    //     while ((strLen = recv(new_fd, buf, MAXBUF, 0)) != 0)
    //     {
    //         if(-1==strLen) //Maybe clinet has been disconnect
    //             break;  //error occurred
    //         buf[strLen]='\0';
    //         printf("Msg from Client:%s",buf);
    //         send(new_fd, buf, strlen(buf), 0);
    //     }
    //     close(new_fd);
        
    // }
 
    // close(sockfd);
    return 0;
}