#include <iostream>
#include <coroutine>
#include <vector>
#include <cstdlib>

#include <windows.h>
#include <fileapi.h>

void DoSomeWork();


struct simple_coroutine {
    struct coroutine_promise {
        int coroutineResult; // This holds the value that will be returned by the coroutine.
		bool is_finished = false;
        simple_coroutine get_return_object() {
            std::cout << "Promise created\n";
            return simple_coroutine(*this);
        }

        std::suspend_always initial_suspend()
        {
            std::cout << "How coroutine should behave right after creation?\n";
            return {};
        }

        std::suspend_always final_suspend() noexcept
        {
            std::cout << "How behave after finishing?\n";
            return {};
        }

        // required for co_yield
        std::suspend_always yield_value(int r) {
            coroutineResult = r;
            return {};
        }

        void return_void()
        {
            std::cout << "Coroutine returns\n";
        }

        void unhandled_exception()
        {
            std::cout << "How to handle en exception in coroutine?";
            terminate();
        }

    };

    using handle_type = std::coroutine_handle<coroutine_promise>;
    using promise_type = coroutine_promise;

    handle_type coro_handle;

    simple_coroutine(promise_type& p) : coro_handle(handle_type::from_promise(p)) 
    {
        std::cout << "Coroutine created\n";
    }

    ~simple_coroutine() {
        std::cout << "Coroutine destroyed\n";
        if (coro_handle) {
            coro_handle.destroy();
			coro_handle = nullptr;
        }
    }

    int get_value() {
        return coro_handle.promise().coroutineResult; // Retrieve the value returned by the coroutine.
    }

    bool next() const {
        coro_handle.resume();
        return !coro_handle.done();
    }
};

simple_coroutine createCoroutine() {
    std::cout << "Coroutine started\n";
    int chunk = 0;
    for (int i = 0; i < 10; i++) 
    {
        // fill result somehow...
        co_yield i*2;
    }
    co_return;
}

int main() {
    auto coroutine = createCoroutine();  // Start the coroutine
    coroutine.coro_handle.resume(); // start coroutine
    while (coroutine.next())
    {
        int result = coroutine.get_value();
        std::cout << "Coroutine yielded value: " << result << std::endl;
    }
    return 0;
}