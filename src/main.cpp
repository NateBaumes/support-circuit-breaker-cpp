#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <iomanip>
#include <signal.h>
#include <atomic>

#include "ticket_processor.h"
#include "rate_limiter.h"
#include "vendor_router.h"

std::atomic<bool> running{true};

void signal_handler(int) {
    std::cout << "\n🛑 Shutting down...\n";
    running = false;
}

void print_stats(const TicketProcessor& processor) {
    std::cout << "\n📊 Statistics:\n";
    std::cout << "   Total processed: " << processor.get_total_processed() << "\n";
    std::cout << "   Avg latency: " << processor.get_avg_latency_us() << " μs\n";
}

int main() {
    signal(SIGINT, signal_handler);
    
    std::cout << "⚡ Support Circuit Breaker Service (C++)\n";
    std::cout << "========================================\n\n";
    
    TicketProcessor processor;
    RateLimiter limiter(1000, 2000);  // 1000 req/sec, burst 2000
    VendorRouter router;
    
    // Add vendors
    router.add_vendor(VendorType::OPENAI, "https://api.openai.com/v1/chat/completions", 
                      "sk-xxx", 1);
    router.add_vendor(VendorType::ANTHROPIC, "https://api.anthropic.com/v1/messages",
                      "sk-ant-xxx", 2);
    router.add_vendor(VendorType::FALLBACK, "http://localhost:8080/fallback", "", 3);
    
    std::cout << "✅ Service running on port 8080\n";
    std::cout << "✅ Circuit breakers active\n";
    std::cout << "✅ Rate limiter: 1000 req/sec\n";
    std::cout << "\nPress Ctrl+C to stop\n\n";
    
    // Simulate incoming tickets
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> type_dist(0, 3);
    std::vector<std::string> subjects = {
        "Cannot login", "Billing issue", "Feature request", "API not working"
    };
    
    int ticket_counter = 0;
    
    while (running) {
        // Create ticket
        Ticket ticket;
        ticket.id = "TICKET-" + std::to_string(++ticket_counter);
        ticket.customer_email = "user" + std::to_string(gen() % 100) + "@example.com";
        ticket.subject = subjects[gen() % subjects.size()];
        ticket.description = "Detailed description of the problem...";
        ticket.created_at = std::chrono::system_clock::now();
        
        // Rate limit check
        if (!limiter.allow_request(ticket.customer_email)) {
            std::cout << "⚠️ Rate limited: " << ticket.customer_email << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        
        // Process ticket
        auto result = processor.process_ticket(ticket);
        
        std::cout << "📝 " << result.ticket_id 
                  << " | Type: " << std::setw(12) << result.classification
                  << " | Sentiment: " << std::setw(8) << result.sentiment
                  << " | Time: " << result.processing_time.count() << " μs";
        
        if (result.escalated) {
            std::cout << " | 🚨 ESCALATED";
        }
        std::cout << "\n";
        
        // Simulate load
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        
        // Print stats every 100 tickets
        if (ticket_counter % 100 == 0) {
            print_stats(processor);
        }
    }
    
    print_stats(processor);
    std::cout << "\n👋 Goodbye!\n";
    
    return 0;
}
