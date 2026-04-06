#ifndef RATE_LIMITER_H
#define RATE_LIMITER_H

#include <unordered_map>
#include <mutex>
#include <chrono>
#include <string>

class RateLimiter {
public:
    RateLimiter(int tokens_per_second = 1000, int max_burst = 2000);
    
    // Returns true if request allowed
    bool allow_request(const std::string& client_id);
    
    // Get current rate for client
    double get_current_rate(const std::string& client_id) const;

private:
    struct TokenBucket {
        double tokens;
        std::chrono::steady_clock::time_point last_refill;
    };
    
    const int tokens_per_second_;
    const int max_burst_;
    std::unordered_map<std::string, TokenBucket> buckets_;
    mutable std::mutex mutex_;
    
    void refill_bucket(TokenBucket& bucket);
};

#endif
