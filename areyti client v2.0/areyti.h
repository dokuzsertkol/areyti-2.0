#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
//#include <thread>
//#include <string>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <fstream>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27019"
#define DEFAULT_BUFLEN 2048

#define CMD_SEND 0x06
#define CMD_END 0x07
#define CMD_PING 0x08
#define CMD_FE_RENAME 0x09

SOCKET ConnectSocket;

// server related
std::string ReceiveMsg() {
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	ZeroMemory(&recvbuf, sizeof(recvbuf));

	int iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0) {
		return std::string(recvbuf, recvbuf + iResult);
	}
	else if (iResult == 0) {
		printf("Connection closing...\n");
		return "";
	}
	else {
		printf("recv failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return "";
	}
}

char ReceiveCommand() {
	char recvbuf[DEFAULT_BUFLEN];
	ZeroMemory(&recvbuf, sizeof(recvbuf));

	int iResult = recv(ConnectSocket, recvbuf, 1, 0);
	if (iResult > 0) {
		return recvbuf[0];
	}
	else if (iResult == 0) {
		printf("Connection closing...\n");
		return 0;
	}
	else {
		printf("recv failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 0;
	}
}

bool ConnectServer(PCSTR ip) {

	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return false;
	}

	struct addrinfo* result = NULL, * ptr = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(ip, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return false;
	}

	ConnectSocket = INVALID_SOCKET;

	ptr = result;
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return false;
	}

	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("Connect failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return false;
	}
	freeaddrinfo(result);

	return true;
}

bool Connect(bool def) {
	std::cout << "Server IP: ";

	std::string ip;
	if (def) {
		ip = "192.168.1.53";
		std::cout << ip << std::endl;
	}
	else std::cin >> ip;

	return ConnectServer(ip.c_str());
}

void Disconnect() {
	// shutdown the send half of the connection since no more data will be sent
	int iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}
	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	return;
}

void SendMsg(std::string msg, bool wait) {
	if (wait && ReceiveMsg()[0] != CMD_SEND) return;
	const char* sendbuf = msg.c_str();

	int iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}
}

void SendCommand(char i, bool wait) {
	if (wait && ReceiveMsg()[0] != CMD_SEND) return;

	const char* sendbuf = &i;

	int iResult = send(ConnectSocket, sendbuf, 1, 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		return;
	}
}

void PingMsg() {
	char c = CMD_PING;
	const char* sendbuf = &c;

	int iResult = send(ConnectSocket, sendbuf, 1, 0);
	if (iResult == SOCKET_ERROR) {
		printf("ping failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}
}
