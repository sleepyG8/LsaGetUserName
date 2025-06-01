#include <Windows.h>
#include <stdio.h>

#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

//reversed format
typedef NTSTATUS(NTAPI* LsaGetUserNameProc)(PUNICODE_STRING*, PUNICODE_STRING*);

int main() {

//Loading...
HMODULE mod = LoadLibrary("Advapi32.dll");
if (!mod) {
    printf("Error loading library\n");
    return 1;
}
//setting pGetusername as LsaGetUserName
LsaGetUserNameProc pGetUserName = (LsaGetUserNameProc)GetProcAddress(mod, "LsaGetUserName");

UNICODE_STRING username = {0};
UNICODE_STRING domain = {0};
PUNICODE_STRING pUsername = &username;
PUNICODE_STRING pDomain = &domain;

//this needed a pointer thus using the above PUNICODE_STRING
NTSTATUS status = pGetUserName(&pUsername, &pDomain);
if (status != STATUS_SUCCESS) {
    printf("error\n");
    return 1;
}

if (pUsername && pUsername->Buffer) {
    wprintf(L"Username: %.*s\n", pUsername->Length / sizeof(WCHAR), pUsername->Buffer);
} else {
    printf("Username buffer is NULL\n");
}

if (pDomain && pDomain->Buffer) {
    wprintf(L"Domain: %.*s\n", pDomain->Length / sizeof(WCHAR), pDomain->Buffer); //%. takes a parameter as a max length but not needed bc Unicodes can sometimes not be null terminated 
} else {
    printf("Domain buffer is NULL\n");
}

return 0;
}
