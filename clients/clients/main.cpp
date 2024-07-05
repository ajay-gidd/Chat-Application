#include<iostream>
#include<WinSock2.h>
#include<ws2tcpip.h>
#include<thread>
#include <string>

using namespace std;
#pragma comment(lib,"ws2_32.lib")

bool initialize()
{
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void SendMessages(SOCKET s)
{
	cout << "Enter Your chat name : " << endl;
	string name;
	getline(cin, name);
	//cin >> name;
	string message;
	while (1)
	{
		getline(cin, message);
		//cin >> message;
		string msg = name + " : " + message;
		int bytesent = send(s, msg.c_str(), msg.length(), 0);
		if (bytesent == SOCKET_ERROR)
		{
			cout << "Error sending message" << endl;
			break;
		}
		if (message == "quit")
		{
			cout << "stopping the application" << endl;
			break;
		}

	}
	closesocket(s);
	WSACleanup();

}

void ReceiveMessage(SOCKET s)
{
	char buffer[4096];
	int recvlen;
	string rmsg = "";
	while (1)
	{
		recvlen = recv(s, buffer, sizeof(buffer), 0);
		if (recvlen <= 0)
		{
			cout << "disconnected" << endl;
			break;
		}
		else
		{
			rmsg = string(buffer, recvlen);
			cout << rmsg << endl;

		}
		

	}
	closesocket(s);
	WSACleanup();

}

int main()
{
	if (!initialize())
	{
		cout << "initialize winsock failed" << endl;
		return 1;

	}

	SOCKET s;

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET) {
		cout << "invalid socket creted" << endl;
		return 1;
	}
	int port = 12345;
	string serveraddress = "127.0.0.1";
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	inet_pton(AF_INET, serveraddress.c_str(), &(serveraddr.sin_addr));

	if (connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {

		cout << "not able to connect to server";
		closesocket(s);
		WSACleanup();
		return 1;


	}
	cout << "connected to server" << endl;
	thread senderthread(SendMessages, s);
	thread receiver(ReceiveMessage, s);

	senderthread.join();
	receiver.join();



	return 0;
}