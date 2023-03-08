#include <atomic>
#include <chrono>
#include <iostream>
#include <semaphore>
#include <thread>

/*
 * g++ -std=c++20 context-switch-latency.cc && ./a.out
 */

// When enabled, we will use the custom spin binary semaphore,
// instead of the C++20 binary_semaphore.
//
// Toggle this on/off to see the difference in "Microseconds per loop"
// when using a classic semaphore vs. spin waiting.
#define USE_SPIN_SEMAPHORE

// constexpr int kPingPongCycles = 1;
constexpr int kPingPongCycles = 1000000;

class spin_binary_semaphore {
  public:
    spin_binary_semaphore(int init = 0) : _value(init) {}
    void acquire() {
        while (true) {
            bool expected = true;
            if (_value.compare_exchange_strong(expected, false)) {
                break;
            }
        }
    }
    void release() { _value = true; }

  private:
    std::atomic<bool> _value;
};

#ifdef USE_SPIN_SEMAPHORE
spin_binary_semaphore ping{0}, pong{0};
#else
std::binary_semaphore ping{0}, pong{0};
#endif

// Sends the first ping.
void fun_ping() {
    for (int i = 0; i < kPingPongCycles; i++) {
        // std::cout << "Ping - " << i << std::endl;
        ping.release(); // Send ping.
        pong.acquire(); // Wait for pong
    }
}

// Sends the response pong.
void fun_pong() {
    for (int i = 0; i < kPingPongCycles; i++) {
        ping.acquire(); // Receive ping.
        // std::cout << "Pong - " << i << std::endl;
        pong.release(); // Send back pong.
    }
}

void fun() {
    // std::cout << "Running fun" << std::endl;

    std::thread th0(fun_ping);
    std::thread th1(fun_pong);
    th0.join();
    th1.join();
}

int main() {
    auto t1 = std::chrono::high_resolution_clock::now();
    fun();
    auto t2 = std::chrono::high_resolution_clock::now();

    auto micros =
        std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
    std::cout << "Total Loop Count: " << kPingPongCycles << std::endl;
    std::cout << "Total Duration: " << micros.count() << "us" << std::endl;

    std::cout << "Duration per Loop: "
              << double(micros.count()) / double(kPingPongCycles) << "us"
              << std::endl;
    return 0;
}