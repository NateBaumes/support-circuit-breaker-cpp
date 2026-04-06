#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <random>

std::atomic<uint64_t> total_requests{0};
std::atomic<uint64_t> successful_requests{0};
std::atomic<uint64_t> failed_requests{0};
std::atomic<bool> running{true};

void worker(int worker_id, int duration_seconds) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> sleep_dist(0, 1000);
    
    auto start = std::chrono::steady_clock::now();
    
    while (running) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            now - start).count();
        
        if (elapsed >= duration_seconds) {
            break;
        }
        
        total_requests++;
        
        // Simulate request to service
        // In production, this would make actual HTTP calls
        
        // Random success/failure
        if (gen() % 100 < 95) {  // 95% success rate
            successful_requests++;
        } else {
            failed_requests++;
        }
        
        // Simulate request latency
        std::this_thread::sleep_for(std::chrono::microseconds(sleep_dist(gen)));
    }
}

int main(int argc, char* argv[]) {
    int num_threads = 8;
    int duration_seconds = 30;
    
    if (argc > 1) num_threads = std::atoi(argv[1]);
    if (argc > 2) duration_seconds = std::atoi(argv[2]);
    
    std::cout << "🚀 Load Test Starting\n";
    std::cout << "   Threads: " << num_threads << "\n";
    std::cout << "   Duration: " << duration_seconds << " seconds\n\n";
    
    std::vector<std::thread> threads;
    
    auto start_time = std::chrono::steady_clock::now();
    
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back(worker, i, duration_seconds);
    }
    
    // Progress indicator
    for (int i = 0; i < duration_seconds; i++) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "\r   Requests: " << total_requests.load() 
                  << " | Success: " << successful_requests.load()
                  << " | Failed: " << failed_requests.load() << std::flush;
    }
    
    running = false;
    
    for (auto& t : threads) {
        t.join();
    }
    
    auto end_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        end_time - start_time).count();
    
    double req_per_sec = static_cast<double>(total_requests.load()) / elapsed;
    
    std::cout << "\n\n📊 Results:\n";
    std::cout << "   Total requests: " << total_requests.load() << "\n";
    std::cout << "   Successful: " << successful_requests.load() << "\n";
    std::cout << "   Failed: " << failed_requests.load() << "\n";
    std::cout << "   Throughput: " << std::fixed << std::setprecision(2) 
              << req_per_sec << " req/sec\n";
    std::cout << "   Success rate: " << std::fixed << std::setprecision(2)
              << (100.0 * successful_requests.load() / total_requests.load()) << "%\n";
    
    return 0;
}
