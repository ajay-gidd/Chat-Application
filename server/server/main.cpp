#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<tchar.h>
#include<thread>
#include<vector>
using namespace std;

#pragma comment(lib,"ws2_32.lib")

bool Intialiaze()
{
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;

}

void InteractWithClien(SOCKET clientSocket, vector<SOCKET>&clients)
{
	//send recv
	char buffer[4096];
	while (1) {
		int bytesrecvd = recv(clientSocket, buffer, sizeof(buffer), 0);

		if (bytesrecvd <= 0)
		{
			cout << "Client disconnected" << endl;
			break;
		}

		string message(buffer, bytesrecvd);
		cout << "Message From Client : " << message << endl;
		for (auto client : clients)
		{
			if (client != clientSocket)
			{
				send(client, message.c_str(), message.length(), 0);

			}
			
		}

	}

	auto it = find(clients.begin(), clients.end(), clientSocket);

	if (it != clients.end())
	{
		clients.erase(it);
	}
	
	

	closesocket(clientSocket);
}

int main()
{
	if (!Intialiaze()) // socket initializing
	{
		cout << "winsock initialization failed";
	}
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0); // creating socket

	if (listenSocket == INVALID_SOCKET)
	{
		cout << "socket creation failed" << endl;
		return 0;
	}

	//create address structure
	int port = 12345;
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);

	if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1) {
		cout << "setting address structure failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//bind 
	if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {

		cout << "Bind Error";
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//listen
	if(listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		cout << "Listen Failed";
		closesocket(listenSocket);
		WSACleanup();
		return 1;

	}

	cout << "Server started listening on port : " << port << endl;

	//start accepting
	vector<SOCKET>clients;

	while (1)
	{
		SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);

		if (clientSocket == INVALID_SOCKET) {
			cout << "INavlid client socket" << endl;
		}
		clients.push_back(clientSocket);


		thread t1(InteractWithClien, clientSocket, std :: ref(clients));
		t1.detach();

	}

	

	
	closesocket(listenSocket);




	WSACleanup(); // cleaning up
	return 0;

}