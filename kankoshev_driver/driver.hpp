#pragma once
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Advapi32.lib")
#include <Windows.h>

#ifndef DRV_R
#   define DRV_R kankoshev::driver::Read
#   define DRV_R_ kankoshev::driver::Read_
#endif

#ifndef DRV_W
#   define DRV_W kankoshev::driver::Write
#   define DRV_W_ kankoshev::driver::Write_
#endif

#ifndef DRV_WP
#   define DRV_WP kankoshev::driver::WriteProtect
#   define DRV_WP_ kankoshev::driver::WriteProtect_
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

        template<typename T = DWORD_PTR, typename T2 = DWORD_PTR> T Read(T2 base) {
            static T out;
            ReadMemoryEx((PVOID)(DWORD_PTR)base, &out, sizeof(T));
            return out;
        }
        template<typename T = DWORD_PTR, typename T2 = DWORD_PTR> bool Read(T2 base, T& buffer) {
            return NT_SUCCESS(ReadMemoryEx((PVOID)(DWORD_PTR)base, &buffer, sizeof(T)));
        }
        template<typename T = DWORD_PTR, typename T2 = DWORD_PTR> bool Write(T2 base, const T& buffer) {
            return NT_SUCCESS(WriteMemoryEx((PVOID)(DWORD_PTR)base, (PVOID)&buffer, sizeof(T)));
        }
        template<typename T = DWORD_PTR, typename T2 = DWORD_PTR> bool WriteProtect(T2 base, const T& buffer) {
            DWORD origProtect = PAGE_EXECUTE_READWRITE;

            if (!NT_SUCCESS(ProtectMemoryEx((PVOID)(DWORD_PTR)base, sizeof(T), &origProtect)))
                return false;

            auto res = NT_SUCCESS(WriteMemoryEx((PVOID)(DWORD_PTR)base, (PVOID)&buffer, sizeof(T)));
            ProtectMemoryEx((PVOID)(DWORD_PTR)base, sizeof(T), &origProtect);
            return res;
        }
        template<typename T = DWORD_PTR> bool Read_(T base, const void* buffer, DWORD_PTR size) {
            return NT_SUCCESS(ReadMemoryEx((PVOID)(DWORD_PTR)base, (PVOID)buffer, size));
        }
        template<typename T = DWORD_PTR> bool Write_(T base, const void* buffer, DWORD_PTR size) {
            return NT_SUCCESS(WriteMemoryEx((PVOID)(DWORD_PTR)base, (PVOID)buffer, size));
        }
        template<typename T = DWORD_PTR> bool WriteProtect_(T base, const void* buffer, DWORD_PTR size) {
            DWORD origProtect = PAGE_EXECUTE_READWRITE;

            if (!NT_SUCCESS(ProtectMemoryEx((PVOID)(DWORD_PTR)base, size, &origProtect)))
                return false;

            auto res = NT_SUCCESS(WriteMemoryEx((PVOID)(DWORD_PTR)base, (PVOID)buffer, size));
            ProtectMemoryEx((PVOID)(DWORD_PTR)base, size, &origProtect);
            return res;
        }

    private:
        NTSTATUS SendSerivce(ULONG ioctl_code, LPVOID io, DWORD size);

    private:
        HANDLE mDriverHandle;
        DWORD mProcessId;
    };

    namespace driver {
        extern Driver* gDriver;

        template<typename T = DWORD_PTR, typename T2 = DWORD_PTR> T Read(T2 base) {
            static T out;
            gDriver->ReadMemoryEx((PVOID)(DWORD_PTR)base, &out, sizeof(T));
            return out;
        }
        template<typename T = DWORD_PTR, typename T2 = DWORD_PTR> bool Read(T2 base, T& buffer) {
            return NT_SUCCESS(gDriver->ReadMemoryEx((PVOID)(DWORD_PTR)base, &buffer, sizeof(T)));
        }
        template<typename T = DWORD_PTR, typename T2 = DWORD_PTR> bool Write(T2 base, const T& buffer) {
            return NT_SUCCESS(gDriver->WriteMemoryEx((PVOID)(DWORD_PTR)base, (PVOID)&buffer, sizeof(T)));
        }
        template<typename T = DWORD_PTR, typename T2 = DWORD_PTR> bool WriteProtect(T2 base, const T& buffer) {
            auto& drv = *gDriver;
            DWORD origProtect = PAGE_EXECUTE_READWRITE;

            if (!NT_SUCCESS(drv.ProtectMemoryEx((PVOID)(DWORD_PTR)base, sizeof(T), &origProtect)))
                return false;

            auto res = NT_SUCCESS(drv.WriteMemoryEx((PVOID)(DWORD_PTR)base, (PVOID)&buffer, sizeof(T)));
            drv.ProtectMemoryEx((PVOID)(DWORD_PTR)base, sizeof(T), &origProtect);
            return res;
        }
        template<typename T = DWORD_PTR> bool Read_(T base, const void* buffer, DWORD_PTR size) {
            return NT_SUCCESS(gDriver->ReadMemoryEx((PVOID)(DWORD_PTR)base, (PVOID)buffer, size));
        }
        template<typename T = DWORD_PTR> bool Write_(T base, const void* buffer, DWORD_PTR size) {
            return NT_SUCCESS(gDriver->WriteMemoryEx((PVOID)(DWORD_PTR)base, (PVOID)buffer, size));
        }
        template<typename T = DWORD_PTR> bool WriteProtect_(T base, const void* buffer, DWORD_PTR size) {
            auto& drv = *gDriver;
            DWORD origProtect = PAGE_EXECUTE_READWRITE;

            if (!NT_SUCCESS(drv.ProtectMemoryEx((PVOID)(DWORD_PTR)base, size, &origProtect)))
                return false;

            auto res = NT_SUCCESS(drv.WriteMemoryEx((PVOID)(DWORD_PTR)base, (PVOID)buffer, size));
            drv.ProtectMemoryEx((PVOID)(DWORD_PTR)base, size, &origProtect);
            return res;
        }
    }
}