// CoroutinesFromScratch.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <exception>
#include <coroutine>
#include <thread>
#include <conio.h>

struct coro_task {
    struct coroutine_promise {
        int coroutineResult;
        coro_task get_return_object() {
            printf("Promise created\n");
            return coro_task(*this);
        }

        std::suspend_never initial_suspend()
        {
            printf("How coroutine should behave right after creation? Start immediately\n");
            return {};
        }

        std::suspend_always final_suspend() noexcept
        {
            printf("How behave after finishing?\n");
            return {};
        }

        std::suspend_always yield_value(int &r) {
            coroutineResult = r;
            return {};
        }


        void return_void()
        {
        }

        void unhandled_exception()
        {
            printf("How to handle en exception in coroutine?\n");
            std::terminate();
        }

    };

    using handle_type = std::coroutine_handle<coroutine_promise>;
    using promise_type = coroutine_promise;

    handle_type coro_handle;

    coro_task(promise_type& p) : coro_handle(handle_type::from_promise(p))
    {
        printf("Coroutine created\n");
    }

    int get_value() {
        return coro_handle.promise().coroutineResult; // Retrieve the value returned by the coroutine.
    }

    bool next() const {
        if (!coro_handle.done()) {
            coro_handle.resume();
            return true;
        }
        return false;
    }

    ~coro_task() {
        printf("Coroutine destroyed\n");
        if (coro_handle) {
            coro_handle.destroy();
            coro_handle = nullptr;
        }
    }
};



coro_task Coroutine()
{
    printf("Coroutine called\n");
    for(int i = 0; i < 10; i++)
    {
        printf("Yield %d\n", i);
        co_yield i;
    }
}

int main()
{
    coro_task enumerator = Coroutine();
    while (enumerator.next())
    {
        int result = enumerator.get_value();
        printf("Coroutine yielded value: %d\n", result);

        auto thread = std::jthread([&enumerator]() {
            if (enumerator.next()) {
                printf("Coroutine yielded value: %d\n", enumerator.get_value());
            }
        });
    }
}