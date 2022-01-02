#include "targetver.h"
#include <fstream>
#include<cstdio>
#include <Windows.h>
#include <iostream>
#include <string>
#include <list>
#include <algorithm>
#include <locale> 
#include <codecvt>
#include"sql_ms.h"

std::string cnvrt(std::wstring);

int main(int argc, char* args[])
{
	HANDLE hPipe = NULL;
	DWORD dwMode = 0;
	BOOL isSuccess = FALSE;
	DWORD iCountIO = 0;

	LPWSTR str = new WCHAR[260];
	memset(str, 0, 259 * 2);

	std::wstring server;
	std::wcout << L"Print name of server, or . for localhost" << std::endl;
	std::getline(std::wcin, server);

	std::wstring pipe;
	std::wcout << L"Print name of pipe:" << std::endl;
	std::getline(std::wcin, pipe);

	std::wstring namePipe(L"\\\\");
	namePipe.append(server);
	namePipe.append(L"\\pipe\\");
	namePipe.append(pipe);
	std::wcout << L"Path to pipe: " << namePipe << std::endl;

	std::wstring username;
	username = L"CALCULATOR";

	int TIMES = 100;
	//namePipe.append(L" ");
	//namePipe.append(username);

	//////////////////////////////////////////////////////////
	WaitNamedPipe(namePipe.c_str(), NMPWAIT_WAIT_FOREVER);

	hPipe = CreateFile(
		namePipe.c_str(),
		GENERIC_ALL,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (hPipe == INVALID_HANDLE_VALUE)
		return EXIT_FAILURE;

	WriteFile(
		hPipe,
		username.c_str(),
		(username.size() + 1) * sizeof(wchar_t),
		&iCountIO,
		NULL);


	while (true)
	{
		WaitNamedPipe(namePipe.c_str(), INFINITE);

		std::wstring message;
		std::wcout << L"Write message: "; // << std::endl;

		int i = (rand() % 6) + 1;
		message = std::to_wstring(i);
		dwMode = PIPE_READMODE_MESSAGE;
		logging_msgs(cnvrt(username), cnvrt(message));
		isSuccess = SetNamedPipeHandleState(
			hPipe,
			&dwMode,
			NULL,
			NULL);
		if (!isSuccess)
		{
			std::wcout << L"Can't edit mode of pipe!";
			return EXIT_FAILURE;
			break;
		}

		std::wcout << L"Sent message to server" << std::endl;
		if (TIMES >= 0) {

			isSuccess = WriteFile(
				hPipe,
				message.c_str(),
				(message.size() + 1) * sizeof(wchar_t),
				&iCountIO,
				NULL);

			if (iCountIO != (message.size() + 1) * sizeof(wchar_t) || !isSuccess)
			{
				std::wcout << L"Error of push message to server!" << std::endl;
				return EXIT_FAILURE;
				break;
			}
			Sleep(10000);
		}

		std::wcout << "Message pushed to server" << std::endl;


	}

	CloseHandle(hPipe);
	delete[] str;
	system("pause");
	return EXIT_SUCCESS;

}

std::string cnvrt(std::wstring tmp_s) {
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	std::string converted_str = converter.to_bytes(tmp_s);
	return converted_str;
}
