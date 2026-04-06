#ifndef TICKET_PROCESSOR_H
#define TICKET_PROCESSOR_H

#include <string>
#include <chrono>
#include <vector>

struct Ticket {
    std::string id;
    std::string customer_email;
    std::string subject;
    std::string description;
    std::chrono::system_clock::time_point created_at;
};

struct TicketResult {
    std::string ticket_id;
    std::string classification;  // billing, technical, etc.
    std::string sentiment;       // positive, neutral, negative
    std::string resolution;
    bool escalated;
    std::chrono::microseconds processing_time;
};

class TicketProcessor {
public:
    TicketProcessor();
    
    // Process single ticket
    TicketResult process_ticket(const Ticket& ticket);
    
    // Batch process (for high throughput)
    std::vector<TicketResult> process_batch(const std::vector<Ticket>& tickets);
    
    // Get stats
    uint64_t get_total_processed() const;
    double get_avg_latency_us() const;

private:
    std::string call_llm_for_classification(const std::string& text);
    std::string call_llm_for_resolution(const std::string& text);
    bool should_escalate(const std::string& sentiment, const std::string& type);
    
    uint64_t total_processed_;
    uint64_t total_latency_us_;
    mutable std::mutex stats_mutex_;
};

#endif
