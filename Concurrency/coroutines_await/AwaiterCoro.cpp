
#include "AwaiterCoro.h"

#include <coroutine>
#include <string>


#include <windows.h>
#include <fileapi.h>


std::vector<uint8_t> ReadFromFileAsync(const std::string& path);


RequestAwaitable::RequestAwaitable(std::string _fileName) : fileName(std::move(_fileName))
{
    result = ReadFromFileAsync(fileName);
}

void RequestAwaitable::await_suspend(std::coroutine_handle<Promise> handle) noexcept
{
	result = ReadFromFileAsync(fileName);
	handle.resume();
	//WaitForSingleObject(handle, INFINITE);
}

std::vector<uint8_t> ReadFromFileAsync(const std::string& path)
{
	
    DWORD bufSize = 1'000'000; // size of each line, in bytes
    std::vector<uint8_t> result(bufSize);
    std::vector<uint8_t> emptyResult;

    HANDLE hFile = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("Error opening file: %s\n", path.c_str());
        return result;
    }

    OVERLAPPED oReadOverlapped = { 0 };

    oReadOverlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!oReadOverlapped.hEvent)
    {
        printf("Error creating I/O event for reading\n");
        return emptyResult;
    }


    result.resize(bufSize);

    if (!ReadFile(hFile, &result[0], bufSize, NULL, &oReadOverlapped))
    {
        if (GetLastError() != ERROR_IO_PENDING)
        {
            printf("Error starting I/O to read\n");
            CloseHandle(oReadOverlapped.hEvent);
            return emptyResult;
        }
    }

    // perform read asynchronously

    DWORD dwWaitRes = WaitForSingleObject(oReadOverlapped.hEvent, INFINITE);

    if (dwWaitRes == WAIT_FAILED)
    {
        printf("Error waiting for I/O to finish\n");
        CancelIo(hFile);
        CloseHandle(oReadOverlapped.hEvent);
        return emptyResult;
    }

    if (oReadOverlapped.hEvent) {
        CloseHandle(oReadOverlapped.hEvent);
    }
    CloseHandle(hFile);

    return result;
}

