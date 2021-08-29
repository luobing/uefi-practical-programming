/**
 * 《UEFI编程实践》随书代码
 * 更多的UEFI探索，可以参考笔者的博客和专栏：
 * CSDN: https://blog.csdn.net/luobing4365
 * 知乎: https://www.zhihu.com/column/c_1233025362843209728
 * **/
// EchoClientTCP4.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>	//InetPton() 头文件

#pragma comment(lib,"ws2_32")//引入库函数

#define BUFFER_SIZE 1024
void ErrorOutput(char * msg);

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET hSocket;
	char message[BUFFER_SIZE];
	int strLen;
	SOCKADDR_IN servAdr;

	if (argc != 3) {
		printf("TCP4 Client by robin.Usage: %s <IP> <port>", argv[0]);
		exit(1);
	}
	else
	{
		printf("Ready to start TCP4 Client. IP=%s,port=%d\n", argv[1], atoi(argv[2]));
	}
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorOutput("Start WSA error,Quit!");
	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
		ErrorOutput("echo_client: socket() error");
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	//servAdr.sin_addr.s_addr = inet_addr(argv[1]);
	inet_pton(AF_INET, argv[1], &(servAdr.sin_addr.s_addr));
	servAdr.sin_port = htons(atoi(argv[2]));

	if (connect(hSocket, (SOCKADDR *)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
		ErrorOutput("connect() error");
	else
		puts("Connect.......");
	while (1)
	{
		fputs("Input message(Q to quit):", stdout);
		fgets(message, BUFFER_SIZE, stdin);

		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break;

		send(hSocket, message, strlen(message), 0);
		strLen = recv(hSocket, message, BUFFER_SIZE - 1, 0);
		message[strLen] = 0;
		printf("Message from server:%s", message);
	}

	closesocket(hSocket);
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