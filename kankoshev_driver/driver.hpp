#pragma once
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Advapi32.lib")
#include <Windows.h>

#ifndef R
#   define R kankoshev::driver::Read
#   define R_ kankoshev::driver::Read_
#endif

#ifndef W
#   define W kankoshev::driver::Write
#   define W_ kankoshev::driver::Write_
#endif

#ifndef WP
#   define WP kankoshev::driver::WriteProtect
#   define WP_ kankoshev::driver::WriteProtect_
#endif

#ifndef NT_SUCCESS
#   define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif

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
        void DetachDriver();
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

    namespace driver {
        extern Driver* gDriver;

        template<typename T = ULONG_PTR, typename T2 = ULONG_PTR> T Read(T2 base) {
            static T out;
            gDriver->ReadMemoryEx((PVOID)(ULONG_PTR)base, &out, sizeof T);
            return out;
        }
        template<typename T = ULONG_PTR, typename T2 = ULONG_PTR> bool Read(T2 base, T& buffer) {
            return NT_SUCCESS(gDriver->ReadMemoryEx((PVOID)(ULONG_PTR)base, &buffer, sizeof T));
        }
        template<typename T = ULONG_PTR, typename T2 = ULONG_PTR> bool Write(T2 base, const T& buffer) {
            return NT_SUCCESS(gDriver->WriteMemoryEx((PVOID)(ULONG_PTR)base, &buffer, sizeof T));
        }
        template<typename T = ULONG_PTR, typename T2 = ULONG_PTR> bool WriteProtect(T2 base, const T& buffer) {
            auto& drv = *gDriver;
            DWORD origProtect = PAGE_EXECUTE_READWRITE;

            if (!NT_SUCCESS(drv.ProtectMemoryEx((PVOID)(ULONG_PTR)base, sizeof T, &origProtect)))
                return false;

            auto res = NT_SUCCESS(drv.WriteMemoryEx((PVOID)(ULONG_PTR)base, &buffer, sizeof T));
            drv.ProtectMemoryEx((PVOID)(ULONG_PTR)base, sizeof T, &origProtect);
            return res;
        }
        template<typename T = ULONG_PTR> bool Read_(T base, PVOID buffer, DWORD size) {
            return NT_SUCCESS(gDriver->ReadMemoryEx((PVOID)(ULONG_PTR)base, buffer, size));
        }
        template<typename T = ULONG_PTR> bool Write_(T base, PVOID buffer, DWORD size) {
            return NT_SUCCESS(gDriver->WriteMemoryEx((PVOID)(ULONG_PTR)base, buffer, size));
        }
        template<typename T = ULONG_PTR> bool WriteProtect_(T base, PVOID buffer, DWORD size) {
            auto& drv = *gDriver;
            DWORD origProtect = PAGE_EXECUTE_READWRITE;

            if (!NT_SUCCESS(drv.ProtectMemoryEx((PVOID)(ULONG_PTR)base, size, &origProtect)))
                return false;

            auto res = NT_SUCCESS(drv.WriteMemoryEx((PVOID)(ULONG_PTR)base, &buffer, size));
            drv.ProtectMemoryEx((PVOID)(ULONG_PTR)base, sizeof T, &origProtect);
            return res;
        }
    }
}