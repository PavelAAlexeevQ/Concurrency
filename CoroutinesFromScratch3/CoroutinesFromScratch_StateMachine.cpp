// CoroutinesFromScratch.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <exception>
#include <coroutine>
#include <thread>
#include <conio.h>

struct coro_task {
    struct coroutine_promise {
        coro_task get_return_object() {
            return coro_task(*this);
        }

        std::suspend_never initial_suspend()
        {
            return {};
        }

        std::suspend_always final_suspend() noexcept
        {
            return {};
        }

        void return_void()
        {
        }

        void unhandled_exception()
        {
            std::terminate();
        }

        coro_task* task;

    };

    using handle_type = std::coroutine_handle<coroutine_promise>;
    using promise_type = coroutine_promise;

    handle_type coro_handle;

    int keyPressed = 0;

    coro_task(promise_type& p) : coro_handle(handle_type::from_promise(p))
    {
        p.task = this;
    }

    ~coro_task() {
        if (coro_handle) {
            coro_handle.destroy();
            coro_handle = nullptr;
        }
    }
};

struct AwaitableResult {
    AwaitableResult(int _waitKey) : waitKey(_waitKey){
    };

    bool await_ready() const noexcept {
        return false;
    }

    void await_suspend(std::coroutine_handle<coro_task::coroutine_promise> _handle) noexcept
    {
		handle = _handle;
    }

    bool await_resume() noexcept {
        return handle.promise().task->keyPressed == waitKey;
    }

    std::coroutine_handle<coro_task::coroutine_promise> handle;
	int waitKey;
};

AwaitableResult AsyncResult(int waitKey) {
    return AwaitableResult(waitKey);
}


coro_task Coroutine()
{
    printf("Statemachine started\n");
    bool nextState;
    printf("Statemachine: initial state -> wait_a \n");
    do {
        nextState = co_await AsyncResult('a');
    } while (nextState == false);
    printf("Statemachine: initial state -> wait_b \n");
    
    do {
        nextState = co_await AsyncResult('b');
    } while (nextState == false);
    printf("Statemachine: initial state -> wait_c \n");

    do {
        nextState = co_await AsyncResult('c');
    } while (nextState == false);
    
    printf("Statemachine: initial state -> final state \n");

    co_return;
}

int main()
{
    coro_task stateMachine = Coroutine();
    do
    {
	    int key = _getch();
        stateMachine.keyPressed = key;
        stateMachine.coro_handle.resume();
    } while (!stateMachine.coro_handle.done());
}
