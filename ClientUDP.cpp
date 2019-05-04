// ClientUDP.cpp : Defines the entry point for the console application.
#include "winsock2.h"
#include <WS2tcpip.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS

using namespace std;

int main(int argc, char* argv[])
{
	const int		BUF_SIZE = 64;
	WSADATA			wsd;				//WSADATA变量
	SOCKET			sClient;			//客户端套接字
	SOCKADDR_IN		servAddr;			//服务器地址
	char			bufSend[BUF_SIZE];	//发送数据缓冲区
	char			bufRecv[BUF_SIZE];  //接收数据缓冲区
	int				retVal;				//返回值
	char*			closeSymbol = "0";//结束通信的标志

									  // 设置服务端地址
	servAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", (void*)&servAddr.sin_addr.S_un.S_addr);
	servAddr.sin_port = htons((short)4999);
	int	nServAddlen = sizeof(servAddr);

	// 初始化套接字动态库
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		cout << "WSAStartup failed!" << endl;
		return -1;
	}

	// 创建服务端套接字
	sClient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == sClient)
	{
		cout << "Socket failed !" << endl;
		WSACleanup();               //释放套接字资源
		return  -1;
	}
	else
	{
		cout << "Client UDP Socket init !" << endl;
	}

	// 循环等待，向服务端发送数据 & 从服务端接收数据
	while (true) 
	{
		// 初始化buf空间
		ZeroMemory(bufSend, BUF_SIZE);

		// 向服务端发送数据buf
		cout << "Data send to Server UDP Socket: ";
		cin >> bufSend;
		retVal = sendto(sClient, bufSend, strlen(bufSend), 0, (sockaddr *)&servAddr, nServAddlen);
		if (SOCKET_ERROR == retVal)
		{
			cout << "send failed!" << endl;
			closesocket(sClient);	//关闭服务端套接字
			WSACleanup();		    //释放套接字资源
			return -1;
		}
		// 若客户端发送的数据是'0'，则表示客户端想结束此次TCP通信
		if (!strcmp(bufSend, closeSymbol))
		{
			cout << "Client UDP Socket wants to finish this communication" << endl;
			break;
		}

		// 从服务端接收数据bufRecv
		retVal = recvfrom(sClient, bufRecv, BUF_SIZE, 0, (sockaddr *)&servAddr, &nServAddlen);
		bufRecv[retVal] = '\0';
		char str[INET_ADDRSTRLEN];
		cout << "Server IP：" << inet_ntop(AF_INET, &servAddr.sin_addr, str, sizeof(str)) << endl;
		cout << "Data recv from Server UDP Socket: " << bufRecv << endl;
		// 若服务端发送的数据是'0'，则表示服务端想结束此次TCP通信	
		if (!strcmp(bufRecv, closeSymbol))
		{
			cout << "Server UDP Socket wants to finish this communication" << endl;
			break;
		}

	}
	//退出
	closesocket(sClient);	//关闭服务端套接字
	WSACleanup();
	//释放套接字资源
	Sleep(5000);
	return 0;
}