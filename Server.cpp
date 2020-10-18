#include "winsock2.h"
#include <WS2tcpip.h>
#include <iostream>
 
#pragma comment(lib, "ws2_32.lib") // load the dll of winsock
using namespace std;

int main(int argc, char* argv[])
{
	const int BUF_SIZE = 64;
	WSADATA			wsd;			    //WSADATA PARAM
	SOCKET			sServer;		    //Server Socket
	SOCKET			sClient;		    //Client Socket
	SOCKADDR_IN		servAddr;		    //Server Addr
	SOCKADDR_IN     clientAddr;         //Client Addr
	SOCKADDR_IN     connect_Addr;       //the Addr that connect this socket (in this demo, connect_Addr = clientAddr)
	int             connectClientlen;
	char			bufSend[BUF_SIZE];	//the buffer area of data-send
	char			bufRecv[BUF_SIZE];  //the buffer area of data-recv
	int				retVal;			    //return value
	char*			closeSymbol = "0";  //symbol of closing communication

    // Server Addr
	servAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", (void*)&servAddr.sin_addr.S_un.S_addr);
	servAddr.sin_port = htons((short)5000);
	int	nServAddlen = sizeof(servAddr);

	// Client Addr
	clientAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", (void*)&clientAddr.sin_addr.S_un.S_addr);
	clientAddr.sin_port = htons((short)4999);
	int	nClientAddlen = sizeof(clientAddr);

	// Initialize the dll of winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		cout << "WSAStartup failed !" << endl;
		return 1;
	}

	// Create the Server socket 
	sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sServer)
	{
		cout << "socket failed!" << endl;
		WSACleanup();			 
		return  -1;
	}
	else
	{
		cout << "Server TCP Socket init!" << endl;
	}

	// Bind the IP and Port on Server Socket 
	retVal = bind(sServer, (LPSOCKADDR)&servAddr, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == retVal)
	{
		cout << "bind failed!" << endl;
		closesocket(sServer);
		WSACleanup();
		Sleep(5000);
		return -1;
	}
	else
	{
		cout << "Server TCP Socket bind IP & Port !" << endl;
		char str[INET_ADDRSTRLEN];
		cout << "Server Address = " << inet_ntop(AF_INET, &servAddr.sin_addr, str, sizeof(str)) << ":" << ntohs(servAddr.sin_port) << endl;
	}

	// Listen the Server socket 
	retVal = listen(sServer, 10);
	if (0 == retVal)
	{
		cout << "Server Socket is listening !" << endl;
	}
	else if (SOCKET_ERROR == retVal)
	{
		cout << "listen failed!" << endl;
		closesocket(sServer);	 
		WSACleanup();	
		Sleep(5000);
		return -1;
	}

	cout << "Server Socket is waiting accpetion !" << endl;
	connectClientlen = sizeof(connect_Addr);
	sClient = accept(sServer, (sockaddr FAR*)&connect_Addr, &connectClientlen);
	if (INVALID_SOCKET == sClient)
	{
		cout << "accept failed!" << endl;
		closesocket(sServer);	
		WSACleanup();
		Sleep(5000);
		return -1;
	}
	else
	{
		char str[INET_ADDRSTRLEN];
		cout << "Server get connection with client socket!" << endl;
		cout << "The Socket that connect Server TCP Socket: [" << inet_ntop(AF_INET, &connect_Addr.sin_addr, str, sizeof(str)) << ":" << ntohs(connect_Addr.sin_port) << "]" << endl;
	}

	// loop for the data from the accpeted socket/send the data to the socket 
	while (TRUE) 
	{
		// Initialize the buffer 
		ZeroMemory(bufRecv, BUF_SIZE);

		// Recevice the buffer data from the client 
		retVal = recv(sClient, bufRecv, BUF_SIZE, 0);
		if (SOCKET_ERROR == retVal)
		{
			continue;
		}

		// Check the data from client 
		bufRecv[retVal] = '\0';			// Set the last bit as \0 to avoid the wrong data  
		cout << "Data recv from Client Socket: " << bufRecv << endl;
		// When the data from client is ¡®0¡¯£¬exit the loop and finish TCP Communication
		if (!strcmp(bufRecv, closeSymbol))
		{
			cout << "Client Socket wants to finish this communication" << endl;
			break;
		}
		else
		{
			// Automatically send the request reply to client  
			char* bufReq  = "Request Reply";
			send(sClient, bufReq, strlen(bufReq), 0);
		}
	}
	// Exit
	closesocket(sServer);	 
	WSACleanup();			 
	Sleep(5000);
	return 0;
}