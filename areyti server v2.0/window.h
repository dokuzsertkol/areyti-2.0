#pragma once

#include <windows.h>
#include <string>
#include <vector>

#define	BACK_TO_CLIENTS 91
#define BACK_TO_MAINMENU 92

#define CLIENTS 10
#define CLIENTS_BUTTON 101

#define MAINMENU 11
#define MAINMENU_FILEEXPLORER 112

#define FILEEXPLORER 12
#define FILEEXPLORER_SELECT 121
#define FILEEXPLORER_IMPORT 122
#define FILEEXPLORER_EXPORT 123
#define FILEEXPLORER_RENAME 124
#define FILEEXPLORER_DELETE 125

std::wstring clientip = L"";
std::vector<HWND> ClientPage, MainMenuPage, FileExplorerPage;
HWND hwnd;
HWND hClients, hClientsList, hClientsError, 
	hMainMenu, hMainMenuClient,
	hFileExplorer, hFileExplorerList, hFileExplorerPath;
int currentPage;

void DebugBox() {
	MessageBox(hwnd, L"", L"", NULL);
}
void DebugBox(LPCWSTR text) {
	MessageBox(hwnd, text, L"Debug", NULL);
}
void DebugBox(LPCWSTR text, LPCWSTR caption) {
	MessageBox(hwnd, text, caption, NULL);
}
void DebugBox(LPCWSTR caption, int text) {
	MessageBox(hwnd, std::to_wstring(text).c_str(), (LPCWSTR)caption, NULL);
}

void AddClientToClientsList(std::wstring client) {
	SendMessage(hClientsList, (UINT)LB_ADDSTRING, (WPARAM)0, (LPARAM)client.c_str());
}

void CreateClientsWindow(HWND hwnd) {
	int w = 0, h = 0;

	hClients = CreateWindow(
		L"STATIC", L"", WS_CHILD,
		w, h, 300, 200, hwnd, (HMENU) NULL,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL
	);
	ClientPage.push_back(hClients);

	ClientPage.push_back(CreateWindow(
		L"STATIC", L"Choose Client", WS_CHILD,
		w + 10, h + 10, 100, 20, hwnd, NULL,
		(HINSTANCE)GetWindowLongPtr(hClients, GWLP_HINSTANCE), NULL
	));

	hClientsList = CreateWindow(
		L"LISTBOX", NULL, 
		LBS_HASSTRINGS | WS_VSCROLL | WS_HSCROLL | WS_BORDER | WS_CHILD | LBS_NOTIFY,
		w + 10, h + 30, 300, 190, hwnd, NULL,
		(HINSTANCE)GetWindowLongPtr(hClients, GWLP_HINSTANCE), NULL
	);
	ClientPage.push_back(hClientsList);

	hClientsError = CreateWindow(
		L"STATIC", L"", WS_CHILD,
		w + 10, h + 220, 230, 20, hwnd, NULL,
		(HINSTANCE)GetWindowLongPtr(hClients, GWLP_HINSTANCE), NULL
	);
	ClientPage.push_back(hClientsError);
}

void CreateMainMenuWindow(HWND hwnd) {
	int w = 0, h = 0, bw = 120, bh = 20;

	hMainMenu = CreateWindow(
		L"STATIC", L"", WS_CHILD,
		w, h, 300, 200, hwnd, NULL,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL
	);
	MainMenuPage.push_back(hMainMenu);

	MainMenuPage.push_back(CreateWindow(
		L"BUTTON", L"<", WS_CHILD,
		w + 10, h + 10, 20, 20, hwnd, (HMENU) BACK_TO_CLIENTS,
		(HINSTANCE)GetWindowLongPtr(hClients, GWLP_HINSTANCE), NULL
	));
	hMainMenuClient = CreateWindow(
		L"STATIC", L"Current Client: ", WS_CHILD,
		w + 40, h + 10, 160, 20, hwnd, NULL,
		(HINSTANCE)GetWindowLongPtr(hClients, GWLP_HINSTANCE), NULL
	);
	MainMenuPage.push_back(hMainMenuClient);

	MainMenuPage.push_back(CreateWindow(
		L"STATIC", L"Choose Operation", WS_CHILD,
		w + 10, h + 40, 160, 20, hwnd, NULL,
		(HINSTANCE)GetWindowLongPtr(hClients, GWLP_HINSTANCE), NULL
	));

	MainMenuPage.push_back(CreateWindow(
		L"BUTTON", L"KeyLogger", WS_CHILD,
		w + 10, h + 70, bw, bh, hwnd, NULL,
		(HINSTANCE)GetWindowLongPtr(hClients, GWLP_HINSTANCE), NULL
	));
	MainMenuPage.push_back(CreateWindow(
		L"BUTTON", L"File Explorer", WS_CHILD,
		w + 20 + bw, h + 70, bw, bh, hwnd, (HMENU)MAINMENU_FILEEXPLORER,
		(HINSTANCE)GetWindowLongPtr(hClients, GWLP_HINSTANCE), NULL
	));
	MainMenuPage.push_back(CreateWindow(
		L"BUTTON", L"Task Manager", WS_CHILD,
		w + 30 + bw * 2, h + 70, bw, bh, hwnd, NULL,
		(HINSTANCE)GetWindowLongPtr(hClients, GWLP_HINSTANCE), NULL
	));

	MainMenuPage.push_back(CreateWindow(
		L"BUTTON", L" ", WS_CHILD,
		w + 10, h + 100, bw, bh, hwnd, NULL,
		(HINSTANCE)GetWindowLongPtr(hClients, GWLP_HINSTANCE), NULL
	));
	MainMenuPage.push_back(CreateWindow(
		L"BUTTON", L" ", WS_CHILD,
		w + 20 + bw, h + 100, bw, bh, hwnd, NULL,
		(HINSTANCE)GetWindowLongPtr(hClients, GWLP_HINSTANCE), NULL
	));
	MainMenuPage.push_back(CreateWindow(
		L"BUTTON", L" ", WS_CHILD,
		w + 30 + bw * 2, h + 100, bw, bh, hwnd, NULL,
		(HINSTANCE)GetWindowLongPtr(hClients, GWLP_HINSTANCE), NULL
	));

	MainMenuPage.push_back(CreateWindow(
		L"BUTTON", L" ", WS_CHILD,
		w + 10, h + 130, bw, bh, hwnd, NULL,
		(HINSTANCE)GetWindowLongPtr(hClients, GWLP_HINSTANCE), NULL
	));
	MainMenuPage.push_back(CreateWindow(
		L"BUTTON", L" ", WS_CHILD,
		w + 20 + bw, h + 130, bw, bh, hwnd, NULL,
		(HINSTANCE)GetWindowLongPtr(hClients, GWLP_HINSTANCE), NULL
	));
	MainMenuPage.push_back(CreateWindow(
		L"BUTTON", L" ", WS_CHILD,
		w + 30 + bw * 2, h + 130, bw, bh, hwnd, NULL,
		(HINSTANCE)GetWindowLongPtr(hClients, GWLP_HINSTANCE), NULL
	));

}

void CreateFileExplorerWindow(HWND hwnd) {
	int w = 0, h = 0;

	hFileExplorer = CreateWindow(
		L"STATIC", L"", WS_CHILD,
		w, h, 300, 200, hwnd, NULL,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL
	);
	FileExplorerPage.push_back(hMainMenu);

	FileExplorerPage.push_back(CreateWindow(
		L"BUTTON", L"<", WS_CHILD,
		w + 10, h + 10, 20, 20, hwnd, (HMENU)BACK_TO_MAINMENU,
		(HINSTANCE)GetWindowLongPtr(hClients, GWLP_HINSTANCE), NULL
	));

	FileExplorerPage.push_back(hMainMenuClient);

	hFileExplorerPath = CreateWindow(
		L"STATIC", L"Path: ", WS_CHILD | WS_BORDER,
		w + 10, h + 40, 300, 20, hwnd, NULL,
		(HINSTANCE)GetWindowLongPtr(hClients, GWLP_HINSTANCE), NULL
	);
	FileExplorerPage.push_back(hFileExplorerPath);

	hFileExplorerList = CreateWindow(
		L"LISTBOX", NULL,
		LBS_HASSTRINGS | WS_VSCROLL | WS_HSCROLL | WS_BORDER | WS_CHILD | LBS_NOTIFY,
		w + 10, h + 60, 300, 190, hwnd, NULL,
		(HINSTANCE)GetWindowLongPtr(hClients, GWLP_HINSTANCE), NULL
	);
	FileExplorerPage.push_back(hFileExplorerList);

	FileExplorerPage.push_back(CreateWindow(
		L"BUTTON", L"Import", WS_CHILD,
		w + 90, h + 240, 60, 20, hwnd, (HMENU)FILEEXPLORER_IMPORT,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL
	));
	FileExplorerPage.push_back(CreateWindow(
		L"BUTTON", L"Rename", WS_CHILD,
		w + 170, h + 240, 60, 20, hwnd, (HMENU)FILEEXPLORER_RENAME,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL
	));
	FileExplorerPage.push_back(CreateWindow(
		L"BUTTON", L"Delete", WS_CHILD,
		w + 250, h + 240, 60, 20, hwnd, (HMENU)FILEEXPLORER_DELETE,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL
	));
	FileExplorerPage.push_back(CreateWindow(
		L"BUTTON", L"Export", WS_CHILD,
		w + 10, h + 240, 60, 20, hwnd, (HMENU)FILEEXPLORER_EXPORT,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL
	));
}