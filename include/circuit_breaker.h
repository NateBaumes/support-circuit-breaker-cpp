#ifndef CIRCUIT_BREAKER_H
#define CIRCUIT_BREAKER_H

#include <atomic>
#include <chrono>
#include <mutex>

enum class CircuitState {
    CLOSED,     // Normal operation
    OPEN,       // Failing, reject requests
    HALF_OPEN   // Testing recovery
};

class CircuitBreaker {
public:
    CircuitBreaker(int failure_threshold = 5, 
                   int recovery_timeout_sec = 60,
                   int half_open_max_calls = 3);
    
    // Returns true if request should proceed
    bool allow_request();
    
    // Record result of request
    void record_success();
    void record_failure();
    
    // Get current state
    CircuitState get_state() const;
    
    // Reset circuit breaker
    void reset();

private:
    const int failure_threshold_;
    const int recovery_timeout_sec_;
    const int half_open_max_calls_;
    
    std::atomic<CircuitState> state_;
    std::atomic<int> failure_count_;
    std::atomic<int> half_open_calls_;
    std::chrono::steady_clock::time_point last_failure_time_;
    mutable std::mutex mutex_;
    
    void transition_to_open();
    void transition_to_half_open();
    void transition_to_closed();
};

#endif
