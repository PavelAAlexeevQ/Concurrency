#include <iostream>
#include <coroutine>
#include <vector>
#include <cstdlib>

#include <windows.h>
#include <fileapi.h>

void DoSomeWork();


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

        // required for co_yield
        std::suspend_always yield_value(std::vector<uint8_t> r) {
            coroutineResult = r;
            return {};
        }

        void return_void()
        {
            printf("Coroutine returns\n");
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
        std::vector<uint8_t>result(i);
        // fill result somehow...
        co_yield result;
    }
    //we may wait for another async operation here
    //auto abotherResult = co_await ReadAnotherDataAsync("another.log", 0, 100'000'000);
    co_return; // Return a value from the coroutine
}

int main() {
    auto coroutine = createCoroutine();  // Start the coroutine
    coroutine.coro_handle.resume(); // start coroutine
    while (coroutine.next())
    {
        std::vector<uint8_t> result = coroutine.get_value();
        printf("Coroutine yielded value size: %zu\n", result.size());
    }
    return 0;
}