#include <iostream>
#include <coroutine>
#include <vector>
#include <cstdlib>

#include <windows.h>
#include <fileapi.h>

void DoSomeWork();

struct AwaitableRead {
    AwaitableRead(std::string _fileName, DWORD offset, DWORD chunkSize) {
        printf("AwaitableRead object created\n");
        
        StartAsyncRead(_fileName, offset, chunkSize);
     };

    bool await_ready() const noexcept {
        printf("AwaitableRead::await_ready()?");
        return resultReady;
    }

    void await_suspend(std::coroutine_handle<> handle) noexcept
    {
        printf("AwaitableRead::await_suspend(), called before suspend coroutine");
    }

    std::vector<uint8_t> await_resume() noexcept {
        WaitForAsyncRead();
        std::cout << "AwaitableRead::await_resume()" << std::endl;
        return result;
    }
    
    std::vector<uint8_t> result;
    bool resultReady = false;
    OVERLAPPED oReadOverlapped = { 0 };
    HANDLE hFile = nullptr;
	DWORD chunkSize = 0;

    void StartAsyncRead(std::string _fileName, DWORD offset, DWORD _chunkSize)
    {
		chunkSize = _chunkSize;
        hFile = CreateFileA(_fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            printf("Error opening file: %s\n", _fileName.c_str());
            resultReady = true;
            return;
        }

        oReadOverlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        if (!oReadOverlapped.hEvent)
        {
            printf("Error creating I/O event for reading\n");
            resultReady = true;
            return;
        }
		oReadOverlapped.Offset = offset;

        result.resize(chunkSize);

        if (!ReadFile(hFile, &result[0], chunkSize, NULL, &oReadOverlapped))
        {
            if (GetLastError() != ERROR_IO_PENDING)
            {
                printf("Error starting I/O to read\n");
                CloseHandle(oReadOverlapped.hEvent);
                resultReady = true;
                return;
            }
        }
    }

    void WaitForAsyncRead()
    {
        DWORD dwWaitRes = WaitForSingleObject(oReadOverlapped.hEvent, INFINITE);
        resultReady = true;
        if (dwWaitRes == WAIT_FAILED)
        {
            printf("Error waiting for I/O to finish\n");
            CancelIo(hFile);
            CloseHandle(oReadOverlapped.hEvent);
            return;
        }
        else
        {
            if (oReadOverlapped.InternalHigh < chunkSize) {
                result.resize(oReadOverlapped.Internal);
            }
        }

        if (oReadOverlapped.hEvent) {
            CloseHandle(oReadOverlapped.hEvent);
        }
        CloseHandle(hFile);
    }
};

AwaitableRead ReadDataAsync(std::string fileName, DWORD offset, DWORD chunkSize) {
	return AwaitableRead(fileName, offset, chunkSize);
}


struct simple_coroutine {
    struct coroutine_promise {
        std::vector<uint8_t> coroutineResult; // This holds the value that will be returned by the coroutine.
		bool is_finished = false;
        simple_coroutine get_return_object() {
            printf("Promise created\n");
            return simple_coroutine(*this);
        }

        std::suspend_always initial_suspend()
        {
            printf("How coroutine should behave right after creation?\n");
            return {};
        }

        std::suspend_always final_suspend() noexcept
        {
            printf("How behave after finishing?\n");
            return {};
        }

        void return_value(std::vector<uint8_t> &val)
        {
            printf("Coroutine stores result\n");
            coroutineResult = val;
			is_finished = true;
        }

        void unhandled_exception()
        {
            printf("How to handle en exception in coroutine?\n");
            terminate();
        }

    };

    using handle_type = std::coroutine_handle<coroutine_promise>;
    using promise_type = coroutine_promise;

    handle_type coro_handle;

    simple_coroutine(promise_type& p) : coro_handle(handle_type::from_promise(p)) 
    {
		printf("Coroutine created\n");
    }

    ~simple_coroutine() {
        printf("Coroutine destroyed\n");
        if (coro_handle) {
            coro_handle.destroy();
			coro_handle = nullptr;
        }
    }

    std::vector<uint8_t> get_value() {
        return coro_handle.promise().coroutineResult; // Retrieve the value returned by the coroutine.
    }

};

simple_coroutine createCoroutine() {
    std::cout << "Coroutine started\n";
    AwaitableRead asyncResult = ReadDataAsync("rand_data.log", 0, 100'000'000);
    std::vector<uint8_t> result = co_await asyncResult;

    //we may wait for another async operation here
    //auto abotherResult = co_await ReadAnotherDataAsync("another.log", 0, 100'000'000);
    co_return result; // Return a value from the coroutine
}

int main() {
    auto coroutine = createCoroutine();  // Start the coroutine
    coroutine.coro_handle.resume(); // start coroutine
    DoSomeWork();
	coroutine.coro_handle.resume(); // resume coroutine, like Task.Result in .NET
    printf("Coroutine finished, returned value size: %zu\n", coroutine.get_value().size());
    return 0;
}

void DoSomeWork()
{
    double rand_min = 1;
    for (int i = 0; i < 100'000'000; i++)
    {
        double r = rand() / (double)RAND_MAX;
        rand_min = min(r, rand_min);
    }
}