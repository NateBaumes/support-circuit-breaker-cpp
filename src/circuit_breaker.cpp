#include "circuit_breaker.h"
#include <iostream>

CircuitBreaker::CircuitBreaker(int failure_threshold, 
                               int recovery_timeout_sec,
                               int half_open_max_calls)
    : failure_threshold_(failure_threshold)
    , recovery_timeout_sec_(recovery_timeout_sec)
    , half_open_max_calls_(half_open_max_calls)
    , state_(CircuitState::CLOSED)
    , failure_count_(0)
    , half_open_calls_(0)
    , last_failure_time_(std::chrono::steady_clock::now()) {}

bool CircuitBreaker::allow_request() {
    auto current_state = state_.load();
    
    if (current_state == CircuitState::OPEN) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            now - last_failure_time_).count();
        
        if (elapsed >= recovery_timeout_sec_) {
            transition_to_half_open();
            return true;
        }
        return false;
    }
    
    if (current_state == CircuitState::HALF_OPEN) {
        int calls = half_open_calls_.fetch_add(1);
        if (calls < half_open_max_calls_) {
            return true;
        }
        return false;
    }
    
    return true;  // CLOSED state
}

void CircuitBreaker::record_success() {
    auto current_state = state_.load();
    
    if (current_state == CircuitState::HALF_OPEN) {
        transition_to_closed();
        std::cout << "✅ Circuit breaker CLOSED (recovered)\n";
    }
    
    failure_count_.store(0);
}

void CircuitBreaker::record_failure() {
    auto current_state = state_.load();
    
    int failures = failure_count_.fetch_add(1) + 1;
    last_failure_time_ = std::chrono::steady_clock::now();
    
    if (current_state == CircuitState::CLOSED && failures >= failure_threshold_) {
        transition_to_open();
        std::cout << "🔴 Circuit breaker OPEN after " << failures << " failures\n";
    } else if (current_state == CircuitState::HALF_OPEN) {
        transition_to_open();
        std::cout << "🔴 Circuit breaker OPEN (half-open test failed)\n";
    }
}

CircuitState CircuitBreaker::get_state() const {
    return state_.load();
}

void CircuitBreaker::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    transition_to_closed();
    failure_count_.store(0);
    half_open_calls_.store(0);
}

void CircuitBreaker::transition_to_open() {
    state_.store(CircuitState::OPEN);
    half_open_calls_.store(0);
}

void CircuitBreaker::transition_to_half_open() {
    state_.store(CircuitState::HALF_OPEN);
    half_open_calls_.store(0);
}

void CircuitBreaker::transition_to_closed() {
    state_.store(CircuitState::CLOSED);
    failure_count_.store(0);
    half_open_calls_.store(0);
}
