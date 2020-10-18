// Client.cpp : Defines the entry point for the console application.
#include "winsock2.h"
#include <WS2tcpip.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS

using namespace std;

int main(int argc, char* argv[])
{
	const int BUF_SIZE = 64;
	WSADATA			wsd;			    //WSADATA Param
	SOCKET			sClient;		    //Server Socket 
	SOCKADDR_IN		servAddr;		    //Server Addr 
	SOCKADDR_IN     clientAddr;         //Client Addr
	int				nAddrLen = sizeof(clientAddr);
	int				nAddrLen_s = sizeof(servAddr);
	char			bufSend[BUF_SIZE];	//the buffer area of data-send
	char			bufRecv[BUF_SIZE];  //the buffer area of data-recv
	int				retVal;			    //return value
	char            str[INET_ADDRSTRLEN];
	char*			closeSymbol = "0";  //symbol of close

    // Server Addr
	servAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", (void*)&servAddr.sin_addr.S_un.S_addr);
	servAddr.sin_port = htons((short)5000);

	// Client Addr
	clientAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", (void*)&clientAddr.sin_addr.S_un.S_addr);
	clientAddr.sin_port = htons((short)4999);

	// Initialize the dll of winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		cout << "WSAStartup failed!" << endl;
		return -1;
	}

	// Create the Server socket 
	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sClient)
	{
		cout << "Socket failed !" << endl;
		WSACleanup();       
		Sleep(5000);
		return  -1;
	}
	else
	{
		cout << "Client Socket init !" << endl;
	}

	// Bind the client socket with IP and Port 
	retVal = bind(sClient, (LPSOCKADDR)&clientAddr, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == retVal)
	{
		cout << "bind failed!" << endl;
		closesocket(sClient);
		WSACleanup();
		Sleep(5000);
		return -1;
	}
	else
	{
		cout << "Client TCP Socket bind IP & Port !" << endl;
		cout << "Client Address = " << inet_ntop(AF_INET, &clientAddr.sin_addr, str, sizeof(str)) << ":" << ntohs(clientAddr.sin_port) << endl;
	}

	// client request connect server socket
	retVal = connect(sClient, (LPSOCKADDR)&servAddr, sizeof(servAddr));
	if (SOCKET_ERROR == retVal)
	{
		cout << "Client connect failed!" << endl;
		closesocket(sClient);	 
		WSACleanup();	
		Sleep(5000);
		return -1;
	}
	else
	{
		cout << "Client connect with Server socket" << endl;
	}

	// loop for the data from the connected socket/send the data to the server socket 
	while (true) {
		// Initialize the buffer
		ZeroMemory(bufSend, BUF_SIZE);

		// Send the data from client socket to Server Socket
		cout << "Data send to   Server Socket [" << inet_ntop(AF_INET, &servAddr.sin_addr, str, sizeof(str)) << ":" << ntohs(servAddr.sin_port) << "] : ";
		cin >> bufSend;
		retVal = send(sClient, bufSend, strlen(bufSend), 0);
		if (SOCKET_ERROR == retVal)
		{
			cout << "Send failed!" << endl;
			closesocket(sClient);	 
			WSACleanup();	
			Sleep(5000);
			return -1;
		}
		// When the data from another socket is ¡®0¡¯£¬exit the loop and finish TCP Communication	
		if (!strcmp(bufSend, closeSymbol))
		{
			cout << "Client Socket wants to finish this communication" << endl;
			break;
		}

		// Receive data from server socket
		retVal = recv(sClient, bufRecv, BUF_SIZE, 0);
		bufRecv[retVal] = '\0';
		cout << "Data recv from Server Socket [" << inet_ntop(AF_INET, &servAddr.sin_addr, str, sizeof(str)) << ":" << ntohs(servAddr.sin_port) << "] : " << bufRecv << endl;
	}

	closesocket(sClient);	 
	WSACleanup();		
	Sleep(5000);
	return 0;
}