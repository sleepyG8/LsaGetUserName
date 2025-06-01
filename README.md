#Reverse Engineering LsaGetUserName

#Exploring an Undocumented Windows API

While analyzing DLL exports with get-dll-exports, I discovered an interesting function: `LsaGetUserName`. Given its name, I suspected it interacted with `LSA authentication`, so I decided to reverse engineer its behavior.

How: 
First things first, I have been doing some undocumented work under the hood of Windows and using `get-dll-exports` my custom export scanner.

seeing a juicy Lsa function like LsaGetUserName I intantly went and started looking up this calls structure and how to get the Username this way. After some searching, nothing, not a single write-up on it on ReactOS. So I used Reacts structure to set

```c
NTSTATUS LsaGetUserName(PUNICODE_STRING*, PUNICODE_STRING*);
```
Upon inspection it reutrns 2 strings, not a string and a size. Which is what through me off. I continued on and did the usual stuff by pulling the function name from Advapi32.dll since I knew it was there from the export table.

```c
HMODULE mod = LoadLibrary("Advapi32.dll");
if (!mod) {
    printf("Error loading library\n");
    return 1;
}
//setting pGetusername as LsaGetUserName
LsaGetUserNameProc pGetUserName = (LsaGetUserNameProc)GetProcAddress(mod, "LsaGetUserName");
```

This above section is basically generic and you will notice it a lot in my projects. This is key to working with undocumented APIs. LoadLibrary() is a very deep function. It does a lot, and I can probably make a whole write-up just for it. The interworkings of LoadLibrary() dive into how a process is loaded at runtime and sets all the PE required data like sections, headers, .relocs(headache), loads the imported libs to provide exports.

This next section of code is what actually is doing something and pulling the names and domain name as well which surprised me.

```c

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
```
Notice that I had to use pointers for this to work. P* means that a function is supposed to be a pointer and just following the NTSTATUS defined function up top.

Finally, it prints the username and the domain and in typical Windows fashion the names were wide strings. I should have assumed being as a lot of data from windows is returned in wide strings so I tried that and it worked. I added a max print length to avoid the UNICODE_STRING from not being null terminated, but it works without it too.

```c
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
```
This is just one of so many functions windows tucks away, if you liked this write-up leave a star and join the discord. I am open to helping anyone who is stuck or if your code is just broken hmu, lets try to fix it together. 

-Sleepy :)
