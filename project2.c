// A C++ APPLICATION WITH A CLEAR DEMONSTRATION OF PERFORMANCE GAINS.

#include <iostream>
#include <vector>
#include <thread>
#include <numeric>
#include <chrono>

// Function to sum part of the vector
void partial_sum(const std::vector<int>& data, size_t start, size_t end, long long& result) {
    result = std::accumulate(data.begin() + start, data.begin() + end, 0LL);
}

int main() {
    const size_t N = 100'000'000; // 100 million elements
    std::vector<int> data(N, 1); // Fill with 1's for easy sum

    // -------- Single-threaded summing --------
    auto start1 = std::chrono::high_resolution_clock::now();
    long long sum1 = std::accumulate(data.begin(), data.end(), 0LL);
    auto end1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration<double>(end1 - start1).count();

    std::cout << "Single-threaded sum: " << sum1 << " in " << duration1 << " seconds.\n";

    // -------- Multi-threaded summing --------
    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4; // Fallback if not detectable

    std::vector<long long> results(num_threads, 0);
    std::vector<std::thread> threads;
    size_t chunk_size = N / num_threads;

    auto start2 = std::chrono::high_resolution_clock::now();

    for (unsigned int i = 0; i < num_threads; ++i) {
        size_t chunk_start = i * chunk_size;
        size_t chunk_end = (i == num_threads - 1) ? N : chunk_start + chunk_size;
        threads.emplace_back(partial_sum, std::cref(data), chunk_start, chunk_end, std::ref(results[i]));
    }
    for (auto& t : threads) t.join();

    long long sum2 = std::accumulate(results.begin(), results.end(), 0LL);

    auto end2 = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration<double>(end2 - start2).count();

    std::cout << "Multi-threaded sum: " << sum2 << " in " << duration2 << " seconds (" 
              << num_threads << " threads).\n";

    // -------- Performance Gain --------
    std::cout << "Performance gain: " << (duration1 / duration2) << "x faster.\n";

    return 0;
}
