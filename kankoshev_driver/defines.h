#pragma once

///////////////////////////////////////////////////////////
#define OFFSET_IO_MIRRORE   0x2338
#define FILE_DEVICE_MIRRORE 0x3009
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
#define IOCTL_GET_MODULE_INFORMATION (ULONG)CTL_CODE(FILE_DEVICE_MIRRORE, OFFSET_IO_MIRRORE + 0x0010, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_COPY_MEMORY (ULONG)CTL_CODE(FILE_DEVICE_MIRRORE, OFFSET_IO_MIRRORE + 0x0050, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_PROTECT_MEMORY (ULONG)CTL_CODE(FILE_DEVICE_MIRRORE, OFFSET_IO_MIRRORE + 0x0100, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_ALLOC_MEMORY (ULONG)CTL_CODE(FILE_DEVICE_MIRRORE, OFFSET_IO_MIRRORE + 0x0150, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_FREE_MEMORY (ULONG)CTL_CODE(FILE_DEVICE_MIRRORE, OFFSET_IO_MIRRORE + 0x0200, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
namespace kankoshev {
	typedef struct _SET_MODULE_INFORMATION {
		ULONG pid;
		wchar_t sz_name[32];
	} SET_MODULE_INFORMATION, * PSET_MODULE_INFORMATION;

	/*typedef struct _GET_MODULE_INFORMATION {
		ULONGLONG base_image;
		ULONGLONG size_of_image;
	} GET_MODULE_INFORMATION, * PGET_MODULE_INFORMATION;*/

	typedef struct _COPY_MEMORY {
		ULONGLONG buffer;
		ULONGLONG address;
		ULONGLONG size;
		ULONG pid;
		BOOLEAN write;
	} COPY_MEMORY, * PCOPY_MEMORY;

	typedef struct _PROTECT_MEMORY {
		ULONG pid;
		ULONGLONG address;
		ULONGLONG size;
		ULONGLONG new_protect;
	} PROTECT_MEMORY, * PPROTECT_MEMORY;

	typedef struct _ALLOC_MEMORY {
		ULONG pid;
		ULONGLONG out_address;
		ULONGLONG size;
		ULONG protect;
	} ALLOC_MEMORY, * PALLOC_MEMORY;

	typedef struct _FREE_MEMORY {
		ULONG pid;
		ULONGLONG address;
	} FREE_MEMORY, * PFREE_MEMORY;
}
///////////////////////////////////////////////////////////

