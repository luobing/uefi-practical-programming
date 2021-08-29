/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
// EchoServerUDP6.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>	//InetPton() 头文件

#pragma comment(lib,"ws2_32")//引入库函数

void ErrorOutput(char * msg);
#define BUFFER_SIZE 1024

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET hServSock;
	char message[BUFFER_SIZE];
	char clientIdbuf[300];
	int strLen;
	SOCKADDR_IN6 servAdr;
	//int clntAdrSize;
	int servAdrSize;

	if (argc != 2) {
		printf("UDP6 Server. Usage: %s <port>", argv[0]);
		exit(1);
	}
	else
	{
		printf("Ready to start UDP6 Server. port=%d\n", atoi(argv[1]));
	}
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorOutput("Start WSA error,Quit!");

	hServSock = socket(AF_INET6, SOCK_DGRAM, 0);
	if (hServSock == INVALID_SOCKET)
		ErrorOutput("socket() error!");
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin6_family = AF_INET6;
	servAdr.sin6_addr = in6addr_any;
	servAdr.sin6_port = htons(atoi(argv[1]));

	if (bind(hServSock, (SOCKADDR *)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorOutput("bind() error");
	/*if (listen(hServSock, 10) == SOCKET_ERROR)
	ErrorOutput("listen() error");*/
	//clntAdrSize = sizeof(clntAdr);
	servAdrSize = sizeof(struct sockaddr_in6);

	while (1)
	{
		strLen = recvfrom(hServSock, message, BUFFER_SIZE, 0, (struct sockaddr *)&servAdr, (socklen_t*)&servAdrSize);
		if (strLen>0)
		{
			inet_ntop(AF_INET6, &servAdr.sin6_addr, clientIdbuf, sizeof(clientIdbuf));
			printf("Client ip %s\n", clientIdbuf);
			message[strLen] = '\0';
			printf("Msg from Client:%s", message);
		}
		if (sendto(hServSock, message, strLen, 0, (struct sockaddr *)&servAdr, servAdrSize)<0)
		{
			printf("Error!\n");
			break;
		}

	}
	/*
	for (i = 0; i < 10; i++)
	{
	hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSize);
	if (hClntSock == -1)
	ErrorOutput("accept() error");
	else
	printf("Connect client %d (port: %d)\n", i + 1, clntAdr.sin6_port);

	while ((strLen = recv(hClntSock, message, BUFFER_SIZE, 0)) != 0)
	{
	if (strLen < 0)
	break;
	message[strLen] = '\0';
	printf("Receive from client(port:%d):%s\n", clntAdr.sin6_port, message);
	send(hClntSock, message, strLen, 0);
	}
	closesocket(hClntSock);

	}*/
	closesocket(hServSock);
	WSACleanup();

	return 0;
}

//处理错误
void ErrorOutput(char * msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
