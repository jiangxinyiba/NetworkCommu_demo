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
	SOCKADDR_IN     send_Data_Addr;     //the Addr that send the data(in this demo, send_Data_Addr = servAddr)
	int				nAddrLen_server = sizeof(servAddr);
	int				nAddrLen_send = sizeof(send_Data_Addr);
	char			bufSend[BUF_SIZE];	//send buffer
	char			bufRecv[BUF_SIZE];  //receive buffer
	int				retVal;			    //return value
	char*			closeSymbol = "0";  //symbol of close
	char            str[INET_ADDRSTRLEN];

	// Server Addr
	servAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", (void*)&servAddr.sin_addr.S_un.S_addr);
	servAddr.sin_port = htons((short)5000);

	// Client Addr
	clientAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", (void*)&clientAddr.sin_addr.S_un.S_addr);
	clientAddr.sin_port = htons((short)4999);

	// Initialize the socket of dll
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		cout << "WSAStartup failed !" << endl;
		return 1;
	}

	// Create client socket 
	sClient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == sClient)
	{
		cout << "socket failed!" << endl;
		WSACleanup();
		Sleep(5000);
		return  -1;
	}
	else
	{
		cout << "Client UDP Socket init!" << endl;
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
		cout << "Client UDP Socket bind IP & Port !" << endl;
		cout << "Client Address = " << inet_ntop(AF_INET, &clientAddr.sin_addr, str, sizeof(str)) << ":" << ntohs(clientAddr.sin_port) << endl;
	}

	// loop - client socket receive data
	while (true) {
		// Initialize the buffer
		ZeroMemory(bufRecv, BUF_SIZE);

		// Send the request to server
		cout << "Data send to   Server Socket [" << inet_ntop(AF_INET, &servAddr.sin_addr, str, sizeof(str)) << ":" << ntohs(servAddr.sin_port) << "] : ";
		cin >> bufSend;
		sendto(sClient, bufSend, strlen(bufSend), 0, (sockaddr *)&servAddr, nAddrLen_server);

		// When the data from another socket is ¡®0¡¯£¬exit the loop and finish TCP Communication	
		if (!strcmp(bufSend, closeSymbol))
		{
			cout << "Client Socket wants to finish this communication" << endl;
			closesocket(sClient);
			WSACleanup();
			Sleep(5000);
			break;
		}

		// Client socket receive reply from send_Data_Addr(servAddr) 
		retVal = recvfrom(sClient, bufRecv, BUF_SIZE, 0, (sockaddr *)&send_Data_Addr, &nAddrLen_send);
		if (SOCKET_ERROR == retVal)
		{
			cout << "Recv Failed!" << endl;
			closesocket(sClient);
			WSACleanup();
			Sleep(5000);
			return -1;
		}

		// check the data
		bufRecv[retVal] = '\0';			// Set the last bit as \0 to avoid the wrong data
		cout << "Data recv from Server Socket [" << inet_ntop(AF_INET, &send_Data_Addr.sin_addr, str, sizeof(str)) << ":" << ntohs(send_Data_Addr.sin_port) << "] : " << bufRecv << endl;
	}
	return 0;
}
