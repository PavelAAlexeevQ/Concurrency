#include <iostream>
#include <random>
#include <chrono>
#include <ctime>
#include <coroutine>
#include <intrin.h>


template<typename T> T makeRand(T maxRand) {
    if (maxRand <= RAND_MAX) {
        return (rand() % maxRand);
    }
    else {
        int t = maxRand / RAND_MAX  + 1;
        return (((rand() * rand()) << 2) + (rand() << 4) + rand()) % maxRand;
    }

}

struct MemBuf {
    MemBuf(uint32_t s) : size(s), offsets(new uint32_t[s]), digits(new int16_t[s]) 
    {
        srand(time(NULL));
        for(size_t i = 0; i < size; i++)
        {
            offsets[i] = makeRand(size);
            digits[offsets[i]] = makeRand(RAND_MAX) - (RAND_MAX/2);
        }
    }
    uint32_t size;
    uint32_t* offsets;
    int16_t* digits;

};

int64_t calcSum(const MemBuf& memBuf) {
    int64_t sum = 0;
    for (size_t i = 0; i < memBuf.size; i++) 
    {
        auto val = memBuf.digits[memBuf.offsets[i]];
		sum += val;
    }
    return sum;
}

struct task {
    struct coroutine_promise {
        int64_t value;
        task get_return_object() {
            return task(*this);
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() {}
        void return_value(int64_t v)
        {
            value = v;
        }
        coroutine_promise(coroutine_promise&& cp) noexcept {
            value = cp.value;
        }
        coroutine_promise() {
            value = 0;
        }
    };
    using handle_type = std::coroutine_handle<coroutine_promise>;
    using promise_type = coroutine_promise;
    handle_type coro_handle;

    task(promise_type& p) : coro_handle(handle_type::from_promise(p)) { }
    task(task&& p){
        coro_handle = p.coro_handle;
		p.coro_handle = nullptr;
    }
    task() {
        ;
    }
    ~task() {
        if (coro_handle) {
            coro_handle.destroy();
        }
    }
private:

};

struct AwaitablePrefetch {
    int16_t* value;
    AwaitablePrefetch(int16_t* v) : value(v) { };
  
    bool await_ready() const noexcept {
        return false;
    }

    void await_suspend(std::coroutine_handle<> handle) noexcept
    {
        _mm_prefetch(reinterpret_cast<const char*>(value), _MM_HINT_NTA);
    }

    int16_t* await_resume() noexcept {
        return value; // this is result of async oparation
    }
};


AwaitablePrefetch prefetch(int16_t* v) {
    return AwaitablePrefetch(v);
}


auto calcSumWithCoroutines(const MemBuf& memBuf, size_t startIndex, size_t endIndex) -> task {
    int64_t sum = 0;
    for (size_t i = startIndex; i < endIndex; i++)
    {
        auto val = co_await prefetch(memBuf.digits + memBuf.offsets[i]);
        sum += *val;
    }
    co_return sum;
}

int main()
{
   typedef std::chrono::high_resolution_clock Clock;
   auto t_start = Clock::now();
   auto memBuf = new MemBuf(1024 * 1024 * 100/*1024*/); //1Gb

   auto t_generated = Clock::now();
   auto sum = calcSum(*memBuf);
   auto t_calcSum = Clock::now();

   std::cout << "Generation time:                "
       << std::chrono::duration_cast<std::chrono::milliseconds>(t_generated - t_start).count()
       << "ms" << std::endl;
   std::cout << "calcSum time:               "
       << std::chrono::duration_cast<std::chrono::milliseconds>(t_calcSum - t_generated).count()
       << "ms" << std::endl;
   std::cout << "Sum      = " << sum << std::endl;

   const int minCoroutinesCount = 20;
   const int maxCoroutinesCount = 21;

   for (int coroutinesCount = minCoroutinesCount; coroutinesCount < maxCoroutinesCount; coroutinesCount++)
   {
       std::vector<task> tasks;
       std::cout << "Coroutines count: " << coroutinesCount << std::endl;
	   auto t_start = Clock::now();
       int currentTaskNum = 0;
       for (int c = 0; c < coroutinesCount; c++) {
           tasks.push_back(
               calcSumWithCoroutines(*memBuf, memBuf->size / coroutinesCount * c, memBuf->size / coroutinesCount * (c + 1)));
       }
       do
       {
           for (int c = 0; c < coroutinesCount; c++) {
               tasks[c].coro_handle.resume();
           }
       } while (!tasks[0].coro_handle.done());

       int64_t sumCoro = 0;
       for (auto i = tasks.begin(); i != tasks.end(); i++) {
           sumCoro += (*i).coro_handle.promise().value;
       };
       auto t_end = Clock::now();
       std::cout << "Sum calcSumWithCoroutines time: "
           << std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count()
           << "ms" << std::endl;
       std::cout << "Sum Coro = " << sumCoro << std::endl;
   }

   
      
/*   std::cout << "Sum      = " << sum << std::endl;*/

}


