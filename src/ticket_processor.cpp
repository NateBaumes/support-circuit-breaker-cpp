#include "ticket_processor.h"
#include <iostream>
#include <random>
#include <thread>

TicketProcessor::TicketProcessor() 
    : total_processed_(0)
    , total_latency_us_(0) {}

TicketResult TicketProcessor::process_ticket(const Ticket& ticket) {
    auto start = std::chrono::high_resolution_clock::now();
    
    TicketResult result;
    result.ticket_id = ticket.id;
    
    // Simulate LLM call (in production, this calls actual API)
    std::this_thread::sleep_for(std::chrono::microseconds(100));
    
    result.classification = call_llm_for_classification(ticket.description);
    result.sentiment = call_llm_for_resolution(ticket.description);
    result.resolution = "Suggested resolution for " + ticket.subject;
    result.escalated = should_escalate(result.sentiment, result.classification);
    
    auto end = std::chrono::high_resolution_clock::now();
    result.processing_time = std::chrono::duration_cast<std::chrono::microseconds>(
        end - start);
    
    // Update stats
    {
        std::lock_guard<std::mutex> lock(stats_mutex_);
        total_processed_++;
        total_latency_us_ += result.processing_time.count();
    }
    
    return result;
}

std::vector<TicketResult> TicketProcessor::process_batch(
    const std::vector<Ticket>& tickets) {
    
    std::vector<TicketResult> results;
    results.reserve(tickets.size());
    
    for (const auto& ticket : tickets) {
        results.push_back(process_ticket(ticket));
    }
    
    return results;
}

uint64_t TicketProcessor::get_total_processed() const {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    return total_processed_;
}

double TicketProcessor::get_avg_latency_us() const {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    if (total_processed_ == 0) return 0.0;
    return static_cast<double>(total_latency_us_) / total_processed_;
}

std::string TicketProcessor::call_llm_for_classification(const std::string& text) {
    // Simulate classification
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dist(0, 3);
    
    const char* types[] = {"billing", "technical", "feature_request", "other"};
    return types[dist(gen)];
}

std::string TicketProcessor::call_llm_for_resolution(const std::string& text) {
    // Simulate sentiment analysis
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dist(0, 2);
    
    const char* sentiments[] = {"positive", "neutral", "negative"};
    return sentiments[dist(gen)];
}

bool TicketProcessor::should_escalate(const std::string& sentiment, 
                                       const std::string& type) {
    return (sentiment == "negative") || (type == "technical");
}
