#pragma once
#include "phnt/phnt_windows.h"
#include "phnt/phnt.h"

#include <filesystem>
#include <fstream>
#include "shellcode.h"

#define patch_shell   XSW("\\SoftwareDistribution\\Download\\")

#ifndef WAIT_OBJECT_TIME
#	define WAIT_OBJECT_TIME(ms) (ms * -10000)
#endif

SIZE_T random_number(SIZE_T Min, SIZE_T Max) {
	static mt19937 mt{ std::random_device{}() };
	std::uniform_int_distribution<SIZE_T> dist{ Min, Max };
	return dist(mt);
}

SIZE_T random_number(SIZE_T Max) {
	return random_number(0, Max);
}

SIZE_T random_number() {
	return random_number(0, std::numeric_limits<SIZE_T>::max());
}

std::string random_string(size_t length)
{
	std::string str = XSA("QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm1234567890");
	std::string newstr;
	size_t pos;
	while (newstr.size() != length)
	{
		pos = ((random_number() % (str.size() + 1)));
		newstr += str.substr(pos, 1);
	}
	return newstr;
}

std::wstring random_string_w(size_t length)
{
	std::wstring str = XSW("QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm1234567890");
	std::wstring newstr;
	size_t pos;
	while (newstr.size() != length)
	{
		pos = ((random_number() % (str.size() + 1)));
		newstr += str.substr(pos, 1);
	}
	return newstr;
}

std::wstring get_parent(const std::wstring& path)
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

std::wstring get_exe_directory()
{
	wchar_t imgName[MAX_PATH] = { 0 };
	DWORD len = ARRAYSIZE(imgName);
	QueryFullProcessImageNameW(GetCurrentProcess(), 0, imgName, &len);
	std::wstring sz_dir = (std::wstring(get_parent(imgName)) + XSW("\\"));
	return sz_dir;
}

std::filesystem::path get_files_directory() {
	auto sysdir = std::getenv(XSA("SystemRoot"));
	if (!sysdir)
		return {};

    return std::filesystem::path(sysdir) / XSW("SoftwareDistribution") / XSW("Download");

#if 0
	WCHAR system_dir[256];
	GetWindowsDirectoryW(system_dir, 256);
	std::wstring sz_dir = (std::wstring(system_dir) + XSW("\\SoftwareDistribution\\Download\\"));
	return sz_dir;
#endif
}

std::filesystem::path get_random_file_name_directory(std::wstring type_file)
{
	return get_files_directory() / (random_string_w(random_number(4,8)) + type_file);
}

//void run_us_admin(std::std::wstring sz_exe, bool show)
//{
//	ShellExecuteW(NULL, XSW(L"runas"), sz_exe.c_str(), NULL, NULL, show);
//}
//
//void run_us_admin_and_params(std::wstring sz_exe, std::wstring sz_params, bool show)
//{
//	ShellExecuteW(NULL, XSW(L"runas"), sz_exe.c_str(), sz_params.c_str(), NULL, show);
//}

bool create_process(const std::wstring& FileName, const std::wstring& Command = {}, BOOL Wait = TRUE, BOOL NoWindow = TRUE) {
	PROCESS_INFORMATION piProcInfo;
	STARTUPINFOW siStartInfo;

	std::filesystem::path fileDir = std::filesystem::path(FileName).parent_path();

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
		fileDir.wstring().c_str(),		   // use parent's current directory
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

bool drop_mapper(filesystem::path path) {
	std::ofstream f(path, std::ios::binary);
	if (!f.is_open())
		return false;

	f.write(reinterpret_cast<const char*>(shell_mapper), sizeof(shell_mapper));
	return true;

#if 0
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
#endif

	return true;
}

bool drop_driver(std::wstring path)
{
	std::ofstream f(path, std::ios::binary);
	if (!f.is_open())
		return false;

	f.write(reinterpret_cast<const char*>(shell_driver), sizeof(shell_driver));
	return true;

#if 0
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
#endif

	return true;
}

bool SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege) {
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!LookupPrivilegeValue(
		NULL,            // lookup privilege on local system
		lpszPrivilege,   // privilege to lookup 
		&luid)) {        // receives LUID of privilege
		// std::cout << "LookupPrivilegeValue error: " << GetLastError() << '\n';
		return false;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = (bEnablePrivilege) ? SE_PRIVILEGE_ENABLED : 0;

	// Enable the privilege or disable all privileges.
	if (!AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL,
		(PDWORD)NULL)) {
		// std::cout << "AdjustTokenPrivileges error: " << GetLastError() << '\n';
		return false;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
		// std::cout << "The token does not have the specified privilege. \n";
		return false;
	}

	return true;
}

bool DisableVulnerableDriverBlocklist() {
	HKEY hKey;
	LONG result;
	DWORD dwType;
	LPCSTR subKey = XSA("SYSTEM\\CurrentControlSet\\Control\\CI\\Config");
	DWORD value = 0;
	DWORD dwSize = 0;

	// Open the registry key
	result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, subKey, 0, KEY_SET_VALUE | KEY_QUERY_VALUE, &hKey);

	if (result != ERROR_SUCCESS) {
		// std::cerr << "Error opening the registry key: " << result << std::endl;
		return false;
	}

	result = RegQueryValueExA(hKey, XSA("VulnerableDriverBlocklistEnable"), NULL, &dwType, NULL, &dwSize);

	if (result != ERROR_SUCCESS) {
		if (result == ERROR_FILE_NOT_FOUND) {
			// std::cerr << "Key or value not found." << std::endl;
			RegCloseKey(hKey);
			return true;
		} else {
			// std::cerr << "Error opening registry: " << result << std::endl;
			RegCloseKey(hKey);
			return false;
		}
	}

	// Check if data type is string (REG_SZ)
	if (dwType != REG_DWORD) {
		RegCloseKey(hKey);
		return false;
	}

	dwType = REG_DWORD;
	DWORD dwValue = 0;
	dwSize = sizeof(DWORD);

	if (RegGetValueA(hKey, NULL, XSA("VulnerableDriverBlocklistEnable"), RRF_RT_ANY, &dwType, &dwValue, &dwSize) != ERROR_SUCCESS) {
		RegCloseKey(hKey);
		return false;
	}

	if (dwValue == 0) {
        RegCloseKey(hKey);
        return true;
    }

	// Set the value
	result = RegSetValueExA(hKey, XSA("VulnerableDriverBlocklistEnable"), 0, REG_DWORD, reinterpret_cast<BYTE*>(&value), sizeof(value));

	if (result != ERROR_SUCCESS) {
		// std::cerr << "Error setting the registry value: " << result << std::endl;
		RegCloseKey(hKey);
		return false;
	}

	// std::cout << "Successfully set the value to zero." << std::endl;
	// Close the registry key
	RegCloseKey(hKey);
	auto mbResult = MessageBoxW(NULL, XSW("�к���ͧ�����ʵ��췤����������͹��ҹ"), XSW("����͹"), MB_YESNO);

	if (mbResult == IDYES) {
		HANDLE hToken;
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
			//std::cout << "OpenProcessToken error: " << GetLastError() << '\n';
			return 1;
		}

		// Enable the shutdown privilege.
		if (!SetPrivilege(hToken, SE_SHUTDOWN_NAME, TRUE)) {
			//std::cout << "Failed to set privilege: " << GetLastError() << '\n';
			CloseHandle(hToken);
			return 1;
		}

		// Use the privilege to shutdown the system
		if (!InitiateSystemShutdown(NULL, NULL, 0, TRUE, TRUE)) {
			//std::cout << "Failed to initiate system shutdown: " << GetLastError() << '\n';
			CloseHandle(hToken);
			return 1;
		}

		// Sleep(1000);
		ExitWindowsEx(EWX_REBOOT | EWX_FORCEIFHUNG, SHTDN_REASON_MINOR_OTHER);
		TerminateProcess(NtCurrentProcess(), 0);
	} else if (mbResult == IDNO) {
		TerminateProcess(NtCurrentProcess(), 0);
	}

	return false;
}

std::wstring get_files_path()
{
	WCHAR system_dir[256];
	GetWindowsDirectoryW(system_dir, 256);
	return (std::wstring(system_dir) + patch_shell);
}

bool mmap_driver() {
	bool result{};
	auto sz_driver = get_random_file_name_directory(XSW(".sys"));
	auto sz_mapper = get_random_file_name_directory(XSW(".exe"));
	auto sz_params_map = XSW("-map \"") + sz_driver.wstring() + XSW("\" > nul");

	// DeleteFileW(sz_driver.c_str());
	// DeleteFileW(sz_mapper.c_str());

	filesystem::remove(sz_driver);
	filesystem::remove(sz_mapper);

	while (true) {
		if (!DisableVulnerableDriverBlocklist())
			break;

		if (!drop_driver(sz_driver))
			break;

		if (!drop_mapper(sz_mapper))
			break;

		if (!create_process(sz_mapper, sz_params_map))
			break;

		result = true;
		break;
	}

	filesystem::remove(sz_driver);
	filesystem::remove(sz_mapper);

	// DeleteFileW(sz_driver.c_str());
	// DeleteFileW(sz_mapper.c_str());

	return result;
}

HMODULE get_module_handle(std::wstring_view moduleName) {
	PVOID dllHandle = NULL;

	UNICODE_STRING dllName;
	RtlInitUnicodeString(&dllName, moduleName.data());

	if (!NT_SUCCESS(LdrGetDllHandle(NULL, NULL, &dllName, &dllHandle)))
		return NULL;

	return (HMODULE)dllHandle;
}

PROC get_proc_address(HMODULE moduleHandle, string_view procName) {
	PVOID procedureAddress = NULL;

	ANSI_STRING procedureName;
	RtlInitAnsiString(&procedureName, procName.data());
	 
	if (!NT_SUCCESS(LdrGetProcedureAddress((PVOID)moduleHandle, &procedureName, 0, &procedureAddress)))
		return NULL;

	return (PROC)procedureAddress;
}