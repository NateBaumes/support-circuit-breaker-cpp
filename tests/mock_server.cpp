#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <random>

// Simple mock server for testing fallback
// In production, this would be a real HTTP server

int main() {
    std::cout << "🖥️ Mock Fallback Server running on port 8080\n";
    std::cout << "   (Simulates vendor API)\n\n";
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> latency_dist(10, 50);
    std::uniform_int_distribution<> error_dist(0, 100);
    
    int request_count = 0;
    
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Simulate processing
        int latency = latency_dist(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(latency));
        
        request_count++;
        
        // Simulate occasional errors (5%)
        if (error_dist(gen) < 5) {
            std::cout << "❌ Request " << request_count << " failed (500 error)\n";
        } else {
            std::cout << "✅ Request " << request_count << " processed in " 
                      << latency << "ms\n";
        }
    }
    
    return 0;
}
