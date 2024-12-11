#pragma once
#include "iostream"
#include <coroutine>
#include <string>

#include "DataTypes.h"


struct Promise;
struct Task {
    // compiler looks for promise_type
    using promise_type = Promise;
    std::coroutine_handle<Promise> coro;

    Task(std::coroutine_handle<Promise> h) : coro(h) {
		std::cout << "Task::Task";
	}

    ~Task() {
        if (coro)
            coro.destroy();
    }
};


struct Promise {
	// current value of suspended coroutine
	probability_distribution_t val;

	// called by compiler first thing to get coroutine result
	Task get_return_object() {
		return Task{ std::coroutine_handle<Promise>::from_promise(*this) };
	}

	// called by compiler first time co_yield occurs
	std::suspend_always initial_suspend() {
		return {};
	}

	// called by compiler for coroutine without return
	std::suspend_never return_void() {
		return {};
	}

	// called by compiler last thing to await final result
	// coroutine cannot be resumed after this is called
	std::suspend_always final_suspend() noexcept {
		return {};
	}

	void unhandled_exception() { std::terminate(); }
};



struct RequestAwaitable {
	RequestAwaitable(std::string _fileName);

	bool await_ready() const noexcept { 
		return false; 
	}
	void await_suspend(std::coroutine_handle<Promise> handle) noexcept;

	std::vector<uint8_t> await_resume() const noexcept { 
		return result;
	}

	std::string fileName;
	std::vector<uint8_t> result;
};
