#include <atomic>
#include <chrono>
#include <iostream>
// #include <semaphore>
#include <thread>

#include <barrier>

/*
 * g++ -std=c++20 barrier-latency.cc && ./a.out
 */

// constexpr int kPingPongCycles = 1;
constexpr int kPingPongCycles = 1000000;

std::barrier sync_point(2);

// Sends the first ping.
void fun_ping() {
    for (int i = 0; i < kPingPongCycles; i++) {
        sync_point.arrive_and_wait();
    }
}

// Sends the response pong.
void fun_pong() {
    for (int i = 0; i < kPingPongCycles; i++) {
        sync_point.arrive_and_wait();
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