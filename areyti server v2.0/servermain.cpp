#include "areyti.h"

bool isRunning = true;
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	WNDCLASS windowClass = {};
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpszClassName = L"MAIN_WINDOW";
	windowClass.lpfnWndProc = WndProc;
	windowClass.hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(101), IMAGE_ICON, 96, 96, LR_DEFAULTCOLOR);
	if (!RegisterClass(&windowClass)) {
		return 1;
	}
	hwnd = CreateWindow(windowClass.lpszClassName, L"Areyti Server", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 510, 345, 0, 0, hInstance, 0);


	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	isRunning = StartServer();

	while (isRunning) {
		MSG message;
		while (PeekMessage(&message, hwnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		Sleep(10);
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	
	LRESULT result = 0;

	switch (uMsg) {

		case WM_CREATE: {
			CreateClientsWindow(hwnd);
			CreateMainMenuWindow(hwnd);
			CreateFileExplorerWindow(hwnd);

			ChangePage(CLIENTS);
			//ChangePage(FILEEXPLORER);

		} break;

		case WM_CLOSE: {
			isRunning = false;
		} break;

		case WM_DESTROY: {

		} break;

		case WM_SIZE: {
			// Get the new width and height of the main window
			int width = LOWORD(lParam);
			int height = HIWORD(lParam);

			// Resize and reposition child windows (pages)
			if (hClients) MoveWindow(hClients, 0, 0, width, height, TRUE);
			if (hMainMenu) MoveWindow(hMainMenu, 0, 0, width, height, TRUE);
			if (hFileExplorer) MoveWindow(hFileExplorer, 0, 0, width, height, TRUE);
		} break;

		case WM_COMMAND: {

			if (HIWORD(wParam) == LBN_DBLCLK) {
				switch (currentPage) {
					case CLIENTS:
						ClientButtonSubmit();
						break;
				}
			}

			switch (LOWORD(wParam)) {

				case BACK_TO_CLIENTS: {
					ChangePage(CLIENTS);
				} break;
				case BACK_TO_MAINMENU: {
					if (!PingMsg()) RemoveClient();
					else ChangePage(MAINMENU);
				} break;

				case MAINMENU_FILEEXPLORER: {
					if (!PingMsg()) RemoveClient();
					else ChangePage(FILEEXPLORER);
				} break;

			}
			break;
		}

		default:
			result = DefWindowProc(hwnd, uMsg, wParam, lParam);
			break;
	}

	return result;
}