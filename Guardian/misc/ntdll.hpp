
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ntdll.lib")
#define _CRT_SECURE_NO_WARNINGS

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#endif