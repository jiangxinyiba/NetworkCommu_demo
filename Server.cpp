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
	char			bufSend[BUF_SIZE];	//发送数据缓冲区
	char			bufRecv[BUF_SIZE];  //接收数据缓冲区
	int				retVal;			    //返回值
	char*			closeSymbol = "0";  //结束通信的标志

	// 服务端套接字地址 
	servAddr.sin_family = AF_INET;        //协议
	servAddr.sin_port = htons(4999);      //端口
	//servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");		  //INADDR_ANY
	inet_pton(AF_INET, "127.0.0.1", (void*)&servAddr.sin_addr.S_un.S_addr);

	// 初始化套接字动态库	
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		cout << "WSAStartup failed !" << endl;
		return 1;
	}

	// 创建服务端套接字
	sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sServer)
	{
		cout << "socket failed!" << endl;
		WSACleanup();			 //释放套接字资源;
		return  -1;
	}
	else
	{
		cout << "Server Socket init!" << endl;
		cout << "Server Socket IP: 127.0.0.1" << endl;
		cout << "Server Socket Port: 4999" << endl;
	}

	// 套接字绑定IP和端口
	
	retVal = bind(sServer, (LPSOCKADDR)&servAddr, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == retVal)
	{
		cout << "bind failed!" << endl;
		closesocket(sServer);	//关闭服务端套接字
		WSACleanup();			//释放套接字资源;
		return -1;
	}
	else
	{
		cout << "Server Socket bind IP & Port !" << endl;
	}

	// 开始监听当前套接字端口是否有数据
	retVal = listen(sServer, 1);
	if (0 == retVal)
	{
		cout << "Server Socket is listening !" << endl;
	}
	else if (SOCKET_ERROR == retVal)
	{
		cout << "listen failed!" << endl;
		closesocket(sServer);	//关闭服务端套接字
		WSACleanup();			//释放套接字资源;
		return -1;
	}
	else
	{
		cout << "One Client Socket is connecting !" << endl;
		
	}

	// 如果客户端发送请求，则接受客户端，开始从该客户端读取数据
	cout << "Server Socket is waiting accpetion !" << endl;
	int addrClientlen = sizeof(clientAddr);
	sClient = accept(sServer, (sockaddr FAR*)&clientAddr, &addrClientlen);
	if (INVALID_SOCKET == sClient)
	{
		cout << "accept failed!" << endl;
		closesocket(sServer);	//关闭服务端套接字
		WSACleanup();			//释放套接字资源;
		return -1;
	}
	else
	{
		cout << "Two Sockets are ready for communication !" << endl;
	}

	// 循环等待accept的端口发送数据，从客户端接收数据 & 向客户端发送数据
	while (true) {
		// 初始化缓冲空间
		ZeroMemory(bufRecv, BUF_SIZE);

		// 接收客户端发送的buf信息
		retVal = recv(sClient, bufRecv, BUF_SIZE, 0);
		if (SOCKET_ERROR == retVal)
		{// 接收失败则关闭服务端客户端套接字
			cout << "recv failed!" << endl;
			closesocket(sServer);	//关闭服务端套接字
			WSACleanup();			//释放套接字资源;
			return -1;
		}
			
		// 确认客户端发送的信息
		bufRecv[retVal] = '\0';			// 接收的最后一位设为\0，避免烫烫的问题
		cout << "Data recv from Client Socket: " << bufRecv << endl;
		// 若客户端发送的数据是'0'，则表示客户端想结束此次TCP通信		
		if (!strcmp(bufRecv, closeSymbol))
		{
			cout << "Client Socket wants to finish this communication" << endl;
			break;
		}

		// 将sendBuf信息发送到客户端
		cout << "Data send to Client Socket: ";
		cin >> bufSend;
		send(sClient, bufSend, strlen(bufSend), 0);
		// 若服务端发送的数据是'0'，则表示服务端想结束此次TCP通信	
		if (!strcmp(bufSend, closeSymbol))
		{
			cout << "Server Socket wants to finish this communication" << endl;
			break;
		}
	}
	// 退出
	closesocket(sServer);	//关闭服务端套接字
	WSACleanup();			//释放套接字资源;
	Sleep(5000);
	return 0;
}