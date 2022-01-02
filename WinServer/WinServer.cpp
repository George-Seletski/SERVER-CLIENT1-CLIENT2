#include <Windows.h>
#include <fstream>
#include<cstdio>
#include <list>
#include <algorithm>
#include <locale> 
#include <codecvt>
#include "sqlite_database.h"
#include <iomanip>
#include "sqlite_client_db.h"
#include "logging_cl.h"
#include "targetver.h"
#define BASE_PATH_PIPE L"\\\\.\\pipe\\"
#define SIZE_BUFFER 2048

#define MESSAGE L"Message"
#define SIZE_MSG_BYTES 16
//std::ofstream file;

int rc;
int rc_client;

DWORD WINAPI Server(LPVOID);
void logging(std::wstring);
std::string cnvrt(std::wstring);

int main(int argc, char* params[])
{
	BOOL isConnected = FALSE;
	DWORD dwThreadId = 0;
	HANDLE hPipe = NULL;
	std::wstring namePipe(BASE_PATH_PIPE);
	std::wstring tmp;
	std::wstring t;
	std::list<HANDLE> threads;

	std::cout << "Write name of pipe: " << std::endl;
	std::wcin >> tmp;
	namePipe.append(tmp);
	std::wcout << "Main thread awake creating pipe with path: " << namePipe << std::endl;
	t.assign(L"Main thread awake creating pipe with path: ");
	t.append(namePipe);
	//LOGS_DB
	rc = sqlite3_open(DB_FILE_NAME, &db);
	if (rc) {
		std::cout << "Error code: " << rc << std::endl;
		return rc;
	}

	rc = create_tables();
	rc = prepare_statements();
	if (rc) {
		std::cout << "Error code: " << rc << std::endl;
		return rc;
	}

	//CLIENT_DB
	rc_client = sqlite3_open(DB_FILE_NAME_C, &db_c);        //database for clients
	if (rc_client) {
		std::cout << "Error code (client db): " << rc_client << std::endl;
		return rc_client;
	}
	rc_client = create_tables_c();									//creating database for clients
	rc_client = prepare_statements_c();

	if (rc_client) {
		std::cout << "Error code (client db): " << rc_client << std::endl;
		return rc_client;
	}

	std::string tmp_s = "Server";
	std::string tmp_s2 = "Started";

	logging(L"Server is started.");
	logging(t);
	t.clear();

	logging_cl(tmp_s, tmp_s2);
	while (true)
	{
		//std::wcout << L"Wait for client" << std::endl;
		hPipe = CreateNamedPipe(
			namePipe.c_str(),
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			SIZE_BUFFER,
			SIZE_BUFFER,
			INFINITE,
			NULL);
		if (hPipe == INVALID_HANDLE_VALUE)
		{
			logging(L"Error of creating pipe! Process will be terminated!");
			std::wcout << L"Error of creating pipe! Process will be terminated!\n";
			return EXIT_FAILURE;
		}

		isConnected = ConnectNamedPipe(hPipe, NULL);
		if (isConnected)
		{
			HANDLE hThread = CreateThread(
				NULL,
				NULL,
				Server,
				(LPVOID)hPipe,
				NULL,
				&dwThreadId);
			if (hThread == INVALID_HANDLE_VALUE)
			{
				logging(L"Error of making thread!");
				std::wcout << L"Error of making thread!\n";
			}
			else
			{
				++dwThreadId;
				threads.push_back(hThread);
			}
		}
		else
		{
			logging(L"Error of connection user");
			tmp_s = "USER";
			tmp_s2 = "Error of connection user";

			logging_cl(tmp_s, tmp_s2);

			std::wcout << L"Error of connection user\n";
			CloseHandle(hPipe);
		}
	}

	std::for_each(threads.cbegin(), threads.cend(),
		[](HANDLE h)
		{
			CloseHandle(h);
		});
	CloseHandle(hPipe);
	sqlite3_close(db);
	return EXIT_SUCCESS;
}

DWORD WINAPI Server(LPVOID hPipe)
{
	LPWSTR strRequest = new WCHAR[SIZE_BUFFER + 1];
	memset(strRequest, 0, (SIZE_BUFFER + 1) * sizeof(WCHAR));
	DWORD cntBytesRead = 0;
	DWORD cntBytesWrited = 0;
	BOOL isSucsses = FALSE;

	std::wstring tmp;
	std::wstring tmp_str;

	HANDLE pipe = (HANDLE)hPipe;
	LPWSTR username = new WCHAR[SIZE_BUFFER + 1];

	isSucsses = ReadFile(
		pipe,
		username,
		SIZE_BUFFER * sizeof(WCHAR),
		&cntBytesRead,
		NULL);

	std::wcout << L"Client " << username << L" connected\n";
	tmp.assign(L"Client ");
	tmp.append(username);
	tmp.append(L" connected");
	logging(tmp);

	std::string tmp_s = "Conncected";

	logging_cl(cnvrt(username), tmp_s);

	tmp.clear();
	logging(L"Instance created, and wait for messages");
	std::wcout << L"Instance created, and wait for messages\n";

	while (true)
	{
		isSucsses = ReadFile(
			pipe,
			strRequest,
			SIZE_BUFFER * sizeof(WCHAR),
			&cntBytesRead,
			NULL);

		if (!isSucsses)
		{
			tmp.assign(L"User ");
			tmp.append(username);
			tmp.append(L" disconnect");
			logging(tmp);

			//tmp_s = "Disconnected";
			//logging_cl(cnvrt(username), tmp_s);

			tmp.clear();
			std::wcout << "User " << username << " disconnect\n";
			DisconnectNamedPipe(hPipe);
			break;
		}
		else
		{
			tmp.assign(username);
			tmp.append(L": ");
			tmp.append(strRequest);
			tmp.append(L"  ::  ");
			std::wcout << username << ": " << strRequest << "  :: ";
			tmp_str.assign(L"got message from");
			tmp_str.append(username);
			logging_cl("Server", cnvrt(tmp_str));
			tmp_str.clear();
		}

		isSucsses = WriteFile(
			pipe,
			MESSAGE,
			SIZE_MSG_BYTES,
			&cntBytesWrited,
			NULL
		);

		tmp_s = "Server";
		std::string tmp_s2;
		tmp_str.assign(L"Message will be replied to ");
		tmp_str.append(username);


		if (!isSucsses || cntBytesWrited != SIZE_MSG_BYTES)
		{
			tmp.append(L"Error of reply!");
			tmp_s2 = "Error of reply!";
			logging(tmp);
			logging_cl(tmp_s, tmp_s2);
			tmp.clear();
			std::wcout << L"Error of reply!\n";
			break;
		}
		else
		{
			tmp.append(L"Message will be replied");
			logging(tmp);
			logging_cl(tmp_s, cnvrt(tmp_str));
			tmp.clear();
			std::wcout << L"Message will be replied\n";
		}

	}
	std::string tmp_s2 = "Finished";

	logging(L"Server is finished.");
	logging_cl(cnvrt(username), tmp_s2);
	logging_cl("Server", "Finished");
	if (strRequest)
		delete[] strRequest;
	CloseHandle(pipe);
	ExitThread(0);

	

}

void logging(std::wstring st)
{
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	std::string converted_str = converter.to_bytes(st);

	rc = insert_log(converted_str);
}

std::string cnvrt(std::wstring tmp_s) {
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	std::string converted_str = converter.to_bytes(tmp_s);
	return converted_str;
}

