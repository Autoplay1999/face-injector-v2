#include "pch.h"
#include "driver.hpp"
#include "defines.h"
#include "api.h"

#define DVR_DEVICE_FILE xor_w(L"\\\\.\\EIQDV") 

namespace kankoshev {
    namespace driver {
        Driver* gDriver;
    }

    Driver::Driver() : mDriverHandle(INVALID_HANDLE_VALUE), mProcessId() {
        driver::gDriver = this;
    }

    Driver::~Driver() {
        if (mDriverHandle != INVALID_HANDLE_VALUE && mDriverHandle != 0) {
            NtClose(mDriverHandle);
            mDriverHandle = INVALID_HANDLE_VALUE;
        }
    }

    bool Driver::DriverInitialize() {
        return mmap_driver();
    }

    NTSTATUS Driver::SendSerivce(ULONG ioctl_code, LPVOID io, DWORD size) {
        if (!IsLoaded())
            return STATUS_DEVICE_DOES_NOT_EXIST;

        if (!DeviceIoControl(mDriverHandle, ioctl_code, io, size, nullptr, 0, NULL, NULL))
            return STATUS_UNSUCCESSFUL;

        return STATUS_SUCCESS;
    }

    void Driver::AttachProcess(DWORD pid) {
        mProcessId = pid;
    }

    NTSTATUS Driver::GetModuleInformationEx(PCWSTR name, PGET_MODULE_INFORMATION mod) {
        if (mDriverHandle == INVALID_HANDLE_VALUE)
            return STATUS_DEVICE_DOES_NOT_EXIST;
    
        SET_MODULE_INFORMATION req = {};

        req.pid = mProcessId;
        StringCchCopyW(req.sz_name, sizeof(req.sz_name), name);

        if (!DeviceIoControl(mDriverHandle, IOCTL_GET_MODULE_INFORMATION, &req, sizeof(req), mod, sizeof(GET_MODULE_INFORMATION), 0, NULL))
            return STATUS_UNSUCCESSFUL;

        return STATUS_SUCCESS;
    }
    NTSTATUS Driver::ReadMemoryEx(PVOID base, PVOID buffer, DWORD size) {
        COPY_MEMORY req = { 0 };

        req.pid     = mProcessId;
        req.address = (ULONGLONG)(base);
        req.buffer  = (ULONGLONG)(buffer);
        req.size    = (ULONGLONG)size;
        req.write   = FALSE;

        return SendSerivce(IOCTL_COPY_MEMORY, &req, sizeof(req));
    }
    NTSTATUS Driver::WriteMemoryEx(PVOID base, PVOID buffer, DWORD size) {
        COPY_MEMORY req = { 0 };

        req.pid     = mProcessId;
        req.address = (ULONGLONG)base;
        req.buffer  = (ULONGLONG)buffer;
        req.size    = (ULONGLONG)size;
        req.write   = TRUE;

        return SendSerivce(IOCTL_COPY_MEMORY, &req, sizeof(req));
    }
    NTSTATUS Driver::ProtectMemoryEx(PVOID base, DWORD size, PDWORD protection)
    {
        PROTECT_MEMORY req = { 0 };

        req.pid = mProcessId;
        req.address = (ULONGLONG)base;
        req.size = size;
        req.new_protect = (ULONGLONG)protection;

        return SendSerivce(IOCTL_PROTECT_MEMORY, &req, sizeof(req));
    }
    PVOID Driver::AllocMemoryEx(DWORD size, DWORD protect)
    {
        ULONGLONG p_out_address = NULL;
        ALLOC_MEMORY req = { 0 };

        req.pid         = mProcessId;
        req.out_address = (ULONGLONG)(&p_out_address);
        req.size        = size;
        req.protect     = protect;

        SendSerivce(IOCTL_ALLOC_MEMORY, &req, sizeof(req));

        return (PVOID)p_out_address;
    }

    NTSTATUS Driver::FreeMemoryEx(PVOID address) {
        FREE_MEMORY req = { 0 };

        req.pid     = mProcessId;
        req.address = (ULONGLONG)address;

        return SendSerivce(IOCTL_FREE_MEMORY, &req, sizeof(req));
    }

    bool Driver::AttachDriver() {
        DetachDriver();

        mDriverHandle = CreateFileW(DVR_DEVICE_FILE, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

        if (!IsLoaded()) {
            mDriverHandle = NULL;
            return false;
        }

        return true;
    }

    void Driver::DetachDriver() {
        if (IsLoaded()) {
            NtClose(mDriverHandle);
            mDriverHandle = NULL;
        }
    }

    Driver& Driver::GetInstance() {
        static shared_ptr<Driver> instance;

        if (!instance)
            if (!(instance = shared_ptr<Driver>(new Driver())))
                throw exception(xor_a("'Driver' CreateInstance Fail"));

        return *instance;
    }
}