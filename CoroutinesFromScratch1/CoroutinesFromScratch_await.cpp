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
            printf("coro_task created\n");
            return coro_task(*this);
        }

        std::suspend_always initial_suspend()
        {
            printf("How coroutine should behave right after creation? Start suspended\n");
            return {};
        }

        std::suspend_always final_suspend() noexcept
        {
            printf("How behave after finishing?\n");
            return {};
        }

        void return_value(int& val)
        {
            printf("Coroutine stores result\n");
            coroutineResult = val;
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

    ~coro_task() {
        printf("Coroutine destroyed\n");
        if (coro_handle) {
            coro_handle.destroy();
            coro_handle = nullptr;
        }
    }
};

struct AwaitableResult {
    AwaitableResult() {
        printf("AwaitableResult constructor\n");
    };

    bool await_ready() const noexcept {
        printf("AwaitableResult::await_ready()?\n");
        return false;
    }

    void await_suspend(std::coroutine_handle<> handle) noexcept
    {
        printf("AwaitableResult::await_suspend(), called before suspend coroutine\n");
    }

    int await_resume() noexcept {
        printf("AwaitableRead::await_resume()\n");
        return 0xCAFED00D; // this is result of async oparation
    }
};

AwaitableResult AsyncResult() {
    return AwaitableResult();
}


coro_task Coroutine()
{
    printf("Coroutine called\n");
    int coro_frame_val = 0xDEADBEEF;
    auto res = co_await AsyncResult();
    printf("Coroutine resumed, coro_frame value persisted: 0x%X\n", coro_frame_val);
	co_return res;
}

int main()
{
    int stack_val = 0;
	coro_task task1 = Coroutine();
    task1.coro_handle.resume();
    //do some stuff
    //...
    //...
    task1.coro_handle.resume();
    printf("Result = 0x%X\n", task1.coro_handle.promise().coroutineResult);









    //-----------------------

    coro_task task2 = Coroutine();
    printf("Do some stuff\n");
    auto thread = std::jthread([&task2]() {
        task2.coro_handle.resume();
        printf("Result = 0x%X\n", task2.coro_handle.promise().coroutineResult);
    });
    thread.join();
}
