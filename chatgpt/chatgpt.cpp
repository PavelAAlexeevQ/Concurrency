#include <iostream>
#include <coroutine>

struct simple_coroutine {
    struct promise_type {
        int value; // This holds the value that will be returned by the coroutine.

        simple_coroutine get_return_object() {
            return simple_coroutine{ *this };
        }

        std::suspend_always initial_suspend() {
            return {};
        }

        std::suspend_always final_suspend() noexcept {
            return {};
        }

        void return_value(int val) { // `co_return` sends an integer value.
            value = val;
        }

        void unhandled_exception() {
            std::cout << "How to handle en exception in coroutine?" << std::endl;
        }

    };

    using handle_type = std::coroutine_handle<promise_type>;

    handle_type coro_handle;

    simple_coroutine(promise_type& p) : coro_handle(handle_type::from_promise(p)) {}

    ~simple_coroutine() {
        if (coro_handle) coro_handle.destroy();
    }

    int get_value() {
        return coro_handle.promise().value; // Retrieve the value returned by the coroutine.
    }
};

simple_coroutine simple_func() {
    std::cout << "Coroutine started\n";
    //co_await std::cout << "await";
    co_return 42; // Return a value from the coroutine
}

int main() {
    auto coro = simple_func();  // Start the coroutine
    coro.coro_handle.resume();
    std::cout << "Coroutine finished\n";
    std::cout << "Returned value: " << coro.get_value() << std::endl; // Print the value returned by the coroutine
    std::cout << "Done: " << coro.coro_handle.done() << std::endl; // Print the value returned by the coroutine
    coro.coro_handle.destroy();
    std::cout << "coro.coro_handle: " << int(coro.coro_handle.operator bool()) << std::endl; // Print the value returned by the coroutine

    return 0;
}
