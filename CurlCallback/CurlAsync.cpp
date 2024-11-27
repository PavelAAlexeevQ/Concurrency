// скомпилировать пример можно так:
// clang++ -std=c++20 -stdlib=libc++ -lcurl -lstdc++ -Wall -Wextra -Wpedantic ./curl_async.cpp
// или
// g++ ./curl_async.cpp --std=c++20 -lcurl -Wall -Wextra -Wpedantic

#include <atomic>
#include <coroutine>
#include <functional>
#include <iostream>
#include <string>
#include <thread>

#include <curl/curl.h>

class WebClient;
struct Result
{
    int code;
    std::string data;
};

struct RequestAwaitable {
    RequestAwaitable(class WebClient* client_, std::string url_) : client(client_), url(std::move(url_)) {};

    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> handle) noexcept;

    Result await_resume() const noexcept { return result; }

    WebClient* client;
    std::string url;
    Result result;
};


class WebClient
{
public:
    WebClient();
    ~WebClient();

    using CallbackFn = std::function<void(Result result)>;
    void runLoop();
    void stopLoop();
    void performRequest(const std::string& url, CallbackFn cb);
    RequestAwaitable performRequestAsync(const std::string& url);
private:
    struct Request
    {
        CallbackFn callback;
        std::string buffer;
    };

    static size_t writeToBuffer(char* ptr, size_t, size_t nmemb, void* tab)
    {
        auto r = reinterpret_cast<Request*>(tab);
        r->buffer.append(ptr, nmemb);
        return nmemb;
    }

    CURLM* m_multiHandle;
    std::atomic_bool m_break{ false };
};


void RequestAwaitable::await_suspend(std::coroutine_handle<> handle) noexcept
{
    client->performRequest(std::move(url), [handle, this](Result res)
        {
            result = std::move(res);
            handle.resume();
        });
}


WebClient::WebClient()
{
    m_multiHandle = curl_multi_init();
}
WebClient::~WebClient()
{
    curl_multi_cleanup(m_multiHandle);
}

void WebClient::performRequest(const std::string& url, CallbackFn cb)
{
    Request* requestPtr = new Request{ std::move(cb), {} };
    CURL* handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &WebClient::writeToBuffer);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, requestPtr);
    curl_easy_setopt(handle, CURLOPT_PRIVATE, requestPtr);
    curl_multi_add_handle(m_multiHandle, handle);
}


RequestAwaitable WebClient::performRequestAsync(const std::string& url)
{
    int coro_call = 2;
    printf("%x", &coro_call);
    return RequestAwaitable(this, std::move(url));
}

void WebClient::stopLoop()
{
    m_break = true;
    curl_multi_wakeup(m_multiHandle);
}

void WebClient::runLoop()
{
    int msgs_left;
    int still_running = 1;

    while (!m_break) {
        auto res = curl_multi_perform(m_multiHandle, &still_running);
        res = curl_multi_poll(m_multiHandle, nullptr, 0, 1000, nullptr);

        CURLMsg* msg;
        while (!m_break && (msg = curl_multi_info_read(m_multiHandle, &msgs_left)))
        {
            if (msg->msg == CURLMSG_DONE)
            {
                CURL* handle = msg->easy_handle;
                int code;
                Request* requestPtr;
                curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &code);
                curl_easy_getinfo(handle, CURLINFO_PRIVATE, &requestPtr);

                requestPtr->callback({ code, std::move(requestPtr->buffer) });
                curl_multi_remove_handle(m_multiHandle, handle);
                curl_easy_cleanup(handle);
                delete requestPtr;
            }
        }
    }
}

struct promise;
struct Task : std::coroutine_handle<promise>
{
    using promise_type = ::promise;
};
struct promise
{
    Task get_return_object() { return { Task::from_promise(*this) }; }
    std::suspend_always initial_suspend() noexcept { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() {}
};

Task doSomething(WebClient& client)
{
    int coro_stack = 0;
    printf("%x\n", &coro_stack);
    auto r0 = client.performRequestAsync("https://postman-echo.com/get");
    co_await r0;

    auto r1 = co_await client.performRequestAsync("https://postman-echo.com/get");
    std::cout << "Req1 ready: " << r1.code << " - " << r1.data << std::endl;

    auto r2 = co_await client.performRequestAsync("http://httpbin.org/user-agent");
    std::cout << "Req2 ready: " << r2.code << " - " << r2.data << std::endl;
    printf("%x\n", &coro_stack);
}

int main(void)
{
    WebClient client;

    int main_stack = 1;
    printf("%x\n", &main_stack);


    auto t = doSomething(client);
    t.resume();
    
    std::thread worker(std::bind(&WebClient::runLoop, &client));
    
    std::cin.get();
    client.stopLoop();
    worker.join();
};