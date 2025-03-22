#include "areyti.h"



void ReceiveLoop() {
	while (true) {
		Sleep(10);

		char recvbuf[DEFAULT_BUFLEN];
		int recvbuflen = DEFAULT_BUFLEN;
		ZeroMemory(&recvbuf, sizeof(recvbuf));

		int iResult = recv(ConnectSocket, recvbuf, 1, 0);
		if (iResult > 0) {

			switch (recvbuf[0]) {
				case CMD_PING: {
					PingMsg();
				} break;
			}
		}
		else if (iResult == 0) {
			printf("Connection closing...\n");
			return;
		}
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			return;
		}
	}
}

int main() {
	bool askForServerIP = true;

	while (true) {
		system("cls");
		std::cout << "Disconnected\n";

		while (!Connect(!askForServerIP));

		system("cls");
		std::cout << "Connected\n";

		//main loop
		ReceiveLoop();

		Disconnect();
	}
}