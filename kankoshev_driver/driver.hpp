#pragma once
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Advapi32.lib")
#include <Windows.h>

namespace kankoshev {
    class Driver {
        Driver();
        Driver(Driver const&) = delete;
        Driver(Driver&&) = delete;
        Driver& operator=(Driver const&) = delete;
        Driver& operator=(Driver&&) = delete;

    public:
        ~Driver();

    public:
        typedef struct _GET_MODULE_INFORMATION {
            ULONGLONG base_image;
            ULONGLONG size_of_image;
        } GET_MODULE_INFORMATION, * PGET_MODULE_INFORMATION;

    public:
        static Driver& GetInstance();

    public:
        bool DriverInitialize();
        bool AttachDriver();
        void AttachProcess(DWORD pid);

        NTSTATUS GetModuleInformationEx(PCWSTR name, PGET_MODULE_INFORMATION mod);
        NTSTATUS ReadMemoryEx(PVOID base, PVOID buffer, DWORD size);
        NTSTATUS WriteMemoryEx(PVOID base, PVOID buffer, DWORD size);
        NTSTATUS ProtectMemoryEx(PVOID base, DWORD size, PDWORD protection);
        PVOID    AllocMemoryEx(DWORD size, DWORD protect);
        NTSTATUS FreeMemoryEx(PVOID address);

        inline bool IsLoaded() const { return mDriverHandle != INVALID_HANDLE_VALUE && mDriverHandle != 0; }

    private:
        NTSTATUS SendSerivce(ULONG ioctl_code, LPVOID io, DWORD size);

    private:
        HANDLE mDriverHandle;
        DWORD mProcessId;
    };
}