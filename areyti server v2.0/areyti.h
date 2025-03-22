#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "window.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <thread> // for AddClient()
#include <shlobj.h> // for browseFolder()
#include <commdlg.h> // for browseFile()
#include <filesystem>
#include <fstream>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27019"
#define DEFAULT_BUFLEN 2048

#define CMD_FE_LOAD 0x01
#define CMD_FE_SELECT 0x02
#define CMD_FE_IMPORT 0x03
#define CMD_FE_EXPORT 0x04
#define CMD_FE_DELETE 0x05
#define CMD_SEND 0x06
#define CMD_END 0x07
#define CMD_PING 0x08
#define CMD_FE_RENAME 0x09

SOCKET ListenSocket;
std::vector<SOCKET> clientSockets;

void RemoveClient();

// socket functions
std::string ReceiveMsg() {
	int index = SendMessage(hClientsList, LB_GETCURSEL, 0, 0);
	SOCKET ClientSocket = clientSockets.at(index);

	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	ZeroMemory(&recvbuf, sizeof(recvbuf));

	int iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0) {
		return std::string(recvbuf, recvbuf + iResult);
	}
	else if (iResult == 0) {
		DebugBox(L"Connection closing...");
		return "";
	}
	else {
		DebugBox(L"recv failed: ", WSAGetLastError());
		RemoveClient();
		return "";
	}
}

char ReceiveCommand() {
	int index = SendMessage(hClientsList, LB_GETCURSEL, 0, 0);
	SOCKET ClientSocket = clientSockets.at(index);

	char recvbuf[DEFAULT_BUFLEN];
	ZeroMemory(&recvbuf, sizeof(recvbuf));

	int iResult = recv(ClientSocket, recvbuf, 1, 0);
	if (iResult > 0) {
		return recvbuf[0];
	}
	else if (iResult == 0) {
		DebugBox(L"Connection closing...");
		return 0;
	}
	else {
		DebugBox(L"recv failed: ", WSAGetLastError());
		RemoveClient();
		return 0;
	}
}

bool SendMsg(std::string msg) {
	Sleep(10);
	int index = SendMessage(hClientsList, LB_GETCURSEL, 0, 0);
	if (index == -1) return true;
	SOCKET ClientSocket = clientSockets.at(index);

	//std::string str = convert::wstr2str(msg);
	const char* sendbuf = msg.c_str();

	// Send an initial buffer
	int iResult = send(ClientSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		DebugBox(L"send failed: ", WSAGetLastError());
		return false;
	}
	return true;
}

void SendCommand(char i) {
	int index = SendMessage(hClientsList, LB_GETCURSEL, 0, 0);
	if (index == -1) return;
	SOCKET ClientSocket = clientSockets.at(index);

	const char* sendbuf = &i;

	// Send an initial buffer
	int iResult = send(ClientSocket, sendbuf, 1, 0);
	if (iResult == SOCKET_ERROR) {
		DebugBox(L"send failed: ", WSAGetLastError());
		return;
	}
}

bool PingMsg() {
	int index = SendMessage(hClientsList, LB_GETCURSEL, 0, 0);
	if (index == -1) return true;
	SOCKET ClientSocket = clientSockets.at(index);

	char i = CMD_PING;
	const char* sendbuf = &i;

	// Send an initial buffer
	int iResult = send(ClientSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		DebugBox(L"ping failed: ", WSAGetLastError());
		return false;
	}
	std::string response = ReceiveMsg();

	if (response[0] != i) {
		DebugBox(L"ping failed: ", WSAGetLastError());
		return false;
	}

	return true;
}

void AddClient() {
	clientSockets.clear();
	while (true) {
		struct sockaddr_in sa = { 0 }; // for TCP/IP 
		socklen_t socklen = sizeof(sa);
		SOCKET ClientSocket = INVALID_SOCKET;

		ClientSocket = accept(ListenSocket, (struct sockaddr*)&sa, &socklen);
		if (ClientSocket == INVALID_SOCKET) {
			//DebugBox(L"accept failed: ", WSAGetLastError());
			//WSACleanup();

			continue;
		}
		std::string clientip = inet_ntoa(sa.sin_addr);
		std::wstring wclientip(clientip.begin(), clientip.end());

		AddClientToClientsList(wclientip);
		clientSockets.push_back(ClientSocket);
	}

	/*
	areyti::SendMessageToClient(L"", areyti::ClientSocket);
	//main loop
	std::thread receiveth(ReceiveMsg, wclientip, areyti::ClientSocket);
	std::thread sendth(SendMsg, areyti::ClientSocket);

	receiveth.join();
	sendth.join();


	areyti::Disconnect();*/
}

bool StartServer() {
	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		DebugBox(L"WSAStartup failed: " + iResult);
		return false;
	}

	struct addrinfo* result = NULL, * ptr = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		DebugBox(L"getaddrinfo failed: "+ iResult);
		WSACleanup();
		return false;
	}

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		DebugBox(L"Error at socket(): " + WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return false;
	}

	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		DebugBox(L"bind failed with error: " + WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	freeaddrinfo(result);

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		DebugBox(L"Listen failed with error: " + WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	std::thread addClientTh(AddClient);
	addClientTh.detach();
	return true;
}


// window button functions
void ClientsListRefresh() {
}

void ChangePage(int pageToShow) {
	currentPage = pageToShow;

	for (HWND h : ClientPage) {
		ShowWindow(h, SW_HIDE);
	}
	for (HWND h : MainMenuPage) {
		ShowWindow(h, SW_HIDE);
	}
	for (HWND h : FileExplorerPage) {
		ShowWindow(h, SW_HIDE);
	}

	switch (pageToShow) {
		case CLIENTS: {
			for (HWND h : ClientPage) {
				ShowWindow(h, SW_SHOW);
			}
			SetWindowText(hClientsError, L"");
			clientip = L"";
		} break;

		case MAINMENU: {
			for (HWND h : MainMenuPage) {
				ShowWindow(h, SW_SHOW);
			}
		} break;
		case FILEEXPLORER: {
			for (HWND h : FileExplorerPage) {
				ShowWindow(h, SW_SHOW);
			}
			FileExplorerCommand(CMD_FE_LOAD);
		} break;
	}
}

void ClientButtonSubmit() {
	int index = SendMessage(hClientsList, LB_GETCURSEL, 0, 0);

	std::wstring selectedClient = L"";
	if (index != LB_ERR)
	{
		int textLength = SendMessage(hClientsList, LB_GETTEXTLEN, index, 0);
		if (textLength != LB_ERR)
		{
			wchar_t* buffer = new wchar_t[textLength + 1];
			SendMessage(hClientsList, LB_GETTEXT, index, reinterpret_cast<LPARAM>(buffer));

			selectedClient = std::wstring(buffer);
			delete[] buffer;
		}
	}

	if (selectedClient != L"") {
		
		if (!PingMsg()) {
			RemoveClient();
			return;
		}
		SetWindowText(hClientsError, L"");

		ChangePage(MAINMENU);
		clientip = selectedClient;
		std::wstring ws = L"Client: " + clientip;
		SetWindowText(hMainMenuClient, ws.c_str());
	}
	else { // bunu sonra sil
		SetWindowText(hClientsError, L"");
		ChangePage(MAINMENU);
		clientip = L"127.0.0.1";
		std::wstring ws = L"Client (DEF): " + clientip;
		SetWindowText(hMainMenuClient, ws.c_str());
	}
}

void RemoveClient() {
	int index = SendMessage(hClientsList, LB_GETCURSEL, 0, 0);
	clientSockets.erase(clientSockets.begin() + index);
	SendMessage(hClientsList, LB_DELETESTRING, (WPARAM)index, 0);
	ChangePage(CLIENTS);
}
