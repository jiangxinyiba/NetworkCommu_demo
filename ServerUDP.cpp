#include "winsock2.h"
#include <WS2tcpip.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

int main(int argc, char* argv[])
{
	const int BUF_SIZE = 64;
	WSADATA			wsd;			    //WSADATA变量
	SOCKET			sServer;		    //服务器套接字
	SOCKET			sClient;		    //客户端套接字
	SOCKADDR_IN		servAddr;		    //服务器地址
	SOCKADDR_IN     clientAddr;         //客户端地址
	int				nAddrLen = sizeof(clientAddr);
	char			bufSend[BUF_SIZE];	//发送数据缓冲区
	char			bufRecv[BUF_SIZE];  //接收数据缓冲区
	int				retVal;			    //返回值
	char*			closeSymbol = "0";  //结束通信的标志

										// 服务端套接字地址 
	servAddr.sin_family = AF_INET;        //协议
	servAddr.sin_port = htons(4999);      //端口
	inet_pton(AF_INET, "127.0.0.1", (void*)&servAddr.sin_addr.S_un.S_addr);

	// 初始化套接字动态库	
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		cout << "WSAStartup failed !" << endl;
		return 1;
	}

	// 创建服务端套接字
	sServer = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == sServer)
	{
		cout << "socket failed!" << endl;
		WSACleanup();			 //释放套接字资源;
		return  -1;
	}
	else
	{
		cout << "Server UDP Socket init!" << endl;
		cout << "Server UDP Socket IP: 127.0.0.1" << endl;
		cout << "Server UDP Socket Port: 4999" << endl;
	}

	// 套接字绑定IP和端口
	if (SOCKET_ERROR == bind(sServer, (LPSOCKADDR)&servAddr, sizeof(SOCKADDR_IN)))
	{
		cout << "bind failed!" << endl;
		closesocket(sServer);	//关闭服务端套接字
		WSACleanup();			//释放套接字资源;
		return -1;
	}
	else
	{
		cout << "Server UDP Socket bind IP & Port !" << endl;
	}

	// 循环等待其他端口发送数据，从客户端接收数据 & 向客户端发送数据
	while (true) {
		// 初始化缓冲空间
		ZeroMemory(bufRecv, BUF_SIZE);

		// 接收客户端发送的buf信息
		retVal = recvfrom(sServer, bufRecv, BUF_SIZE, 0, (sockaddr *)&clientAddr, &nAddrLen);
		if (SOCKET_ERROR == retVal)
		{// 接收失败则关闭服务端客户端套接字
			cout << "Recv Failed!" << endl;
			closesocket(sServer);	//关闭服务端套接字
			WSACleanup();			//释放套接字资源;
			return -1;
		}

		// 确认客户端发送的信息
		bufRecv[retVal] = '\0';			// 接收的最后一位设为\0，避免烫烫的问题
		char str[INET_ADDRSTRLEN];
		cout << "Client IP：" << inet_ntop(AF_INET, &clientAddr.sin_addr, str, sizeof(str)) << endl;
		cout << "Data recv from Client UDP Socket: " << bufRecv << endl;

		// 若客户端发送的数据是'0'，则表示客户端想结束此次TCP通信		
		if (!strcmp(bufRecv, closeSymbol))
		{
			cout << "Client UDP Socket wants to finish this communication" << endl;
			break;
		}

		// 将sendBuf信息发送到客户端
		cout << "Data send to Client UDP Socket: ";
		cin >> bufSend;
		sendto(sServer, bufSend, strlen(bufSend), 0, (sockaddr *)&clientAddr, nAddrLen);
		// 若服务端发送的数据是'0'，则表示服务端想结束此次TCP通信	
		if (!strcmp(bufSend, closeSymbol))
		{
			cout << "Server UDP Socket wants to finish this communication" << endl;
			break;
		}
	}
	// 退出
	closesocket(sServer);	//关闭服务端套接字
	WSACleanup();			//释放套接字资源;
	Sleep(5000);
	return 0;
}