#include "rate_limiter.h"
#include <algorithm>

RateLimiter::RateLimiter(int tokens_per_second, int max_burst)
    : tokens_per_second_(tokens_per_second)
    , max_burst_(max_burst) {}

bool RateLimiter::allow_request(const std::string& client_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto& bucket = buckets_[client_id];
    refill_bucket(bucket);
    
    if (bucket.tokens >= 1.0) {
        bucket.tokens -= 1.0;
        return true;
    }
    
    return false;
}

double RateLimiter::get_current_rate(const std::string& client_id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = buckets_.find(client_id);
    if (it == buckets_.end()) {
        return 0.0;
    }
    
    // Copy bucket to avoid const issues
    TokenBucket bucket = it->second;
    const_cast<RateLimiter*>(this)->refill_bucket(bucket);
    
    return bucket.tokens;
}

void RateLimiter::refill_bucket(TokenBucket& bucket) {
    auto now = std::chrono::steady_clock::now();
    
    if (bucket.last_refill.time_since_epoch().count() == 0) {
        bucket.tokens = max_burst_;
        bucket.last_refill = now;
        return;
    }
    
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
        now - bucket.last_refill).count() / 1'000'000.0;
    
    double new_tokens = elapsed * tokens_per_second_;
    bucket.tokens = std::min(static_cast<double>(max_burst_), 
                              bucket.tokens + new_tokens);
    bucket.last_refill = now;
}
