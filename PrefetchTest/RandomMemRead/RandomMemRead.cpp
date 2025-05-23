#include <iostream>
#include <random>
#include <chrono>
#include <ctime>
#include <coroutine>
#include <xmmintrin.h>
#include <thread>


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
    MemBuf(uint32_t s) : size(s), v_offsets(s), v_digits(s)
    {
        offsets = &(v_offsets[0]);
        digits = &(v_digits[0]);
        auto const threadsCount = 1;
        srand(time(NULL));
        auto const t = 0;
        std::jthread thr[threadsCount];
        //for (int t = 0; t < threadsCount; t++)
        //{
        //    thr[t] = std::jthread([&, t]() {
                for (size_t i = size / threadsCount * t; i < size / threadsCount * (t + 1); i++)
                {
                    offsets[i] = makeRand(size);
                    digits[offsets[i]] = makeRand(RAND_MAX) - (RAND_MAX / 2);
                }
       //     });
        //}
    }
    std::vector<uint32_t> v_offsets;
    std::vector<int16_t> v_digits;
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

std::vector<task> tasks;
int currentTask = 0;

template<class T> struct AwaitablePrefetch {
    T* value;
    AwaitablePrefetch(T* v) : value(v) 
    { 
    };
  
    bool await_ready() const noexcept {
        return false;
    }

    auto await_suspend(std::coroutine_handle<> handle) noexcept
    {
        _mm_prefetch(reinterpret_cast<const char*>(value), _MM_HINT_NTA);
        currentTask++;
        currentTask = currentTask % tasks.size();
        task& t = tasks[currentTask];
        return t.coro_handle;
    }

    T await_resume() noexcept {
        return std::move(*value); // this is result of async oparation
    }
};


template<class T> AwaitablePrefetch<T> prefetch(T* v) {
    return AwaitablePrefetch(v);
}


auto calcSumWithCoroutines(const MemBuf& memBuf, size_t startIndex, size_t endIndex) -> task {
    int64_t sum = 0;
    for (size_t i = startIndex; i < endIndex; i++)
    {
        auto val = co_await prefetch(memBuf.digits + memBuf.offsets[i]);
        sum += val;
    }
    co_return sum;
}

int main()
{
   typedef std::chrono::high_resolution_clock Clock;
   auto t_start = Clock::now();
   auto memBuf = new MemBuf(1024 * 1024 * 500);
 
   auto t_generated = Clock::now();

   std::cout << "Generation time:                "
       << std::chrono::duration_cast<std::chrono::milliseconds>(t_generated - t_start).count()
       << "ms" << std::endl;

   auto t_calcSumStart = Clock::now();
   auto sum = calcSum(*memBuf);
   auto t_calcSumEnd = Clock::now();

   std::cout << "calcSum time:                   "
       << std::chrono::duration_cast<std::chrono::milliseconds>(t_calcSumEnd - t_calcSumStart).count()
       << "ms" << std::endl;
   
   /*auto t_calcSumPrefetechStart = Clock::now();
   auto sumPrefetch = calcSumWithPrefetch(*memBuf);
   auto t_calcSumPrefetechEnd = Clock::now();
   
   std::cout << "calcSumWithPrefetch time:                   "
       << std::chrono::duration_cast<std::chrono::milliseconds>(t_calcSumPrefetechEnd - t_calcSumPrefetechStart).count()
       << "ms" << std::endl;
   */
   const int coroutinesCount = 16;
    //const int coroutinesCountMax = 100;
    //for (int coroutinesCount = 1; coroutinesCount < coroutinesCountMax; coroutinesCount++) {
        tasks.clear();
        auto t_startCoro = Clock::now();
        int currentTaskNum = 0;
        for (int c = 0; c < coroutinesCount; c++) {
            tasks.push_back(
                calcSumWithCoroutines(*memBuf, memBuf->size / coroutinesCount * c, memBuf->size / coroutinesCount * (c + 1)));
        }

        tasks[0].coro_handle.resume();
        for (int c = 0; c < coroutinesCount; c++) {
            if (!tasks[c].coro_handle.done())
                tasks[c].coro_handle.resume();
        }
        int64_t sumCoro = 0;
        for (auto i = tasks.begin(); i != tasks.end(); i++) {
            sumCoro += (*i).coro_handle.promise().value;
        };
        auto t_end = Clock::now();
        std::cout << "Sum calcSumWithCoroutines(" << coroutinesCount << ") time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_startCoro).count()
            << "ms" << std::endl;
        std::cout << "Coro sum      = " << sumCoro << std::endl;
    //}
    std::cout << "Sum           = " << sum << std::endl;
    //std::cout << "sumPrefetch   = " << sumPrefetch << std::endl;
}


