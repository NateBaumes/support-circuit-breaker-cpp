#ifndef VENDOR_ROUTER_H
#define VENDOR_ROUTER_H

#include <string>
#include <vector>
#include <memory>
#include "circuit_breaker.h"

enum class VendorType {
    OPENAI,
    ANTHROPIC,
    FALLBACK
};

struct VendorEndpoint {
    VendorType type;
    std::string url;
    std::string api_key;
    std::unique_ptr<CircuitBreaker> circuit_breaker;
    int priority;  // lower = higher priority
};

class VendorRouter {
public:
    VendorRouter();
    
    // Add vendor endpoint
    void add_vendor(VendorType type, const std::string& url, 
                    const std::string& api_key, int priority);
    
    // Route request to best available vendor
    std::string route_request(const std::string& ticket_text);
    
    // Get vendor status
    bool is_vendor_healthy(VendorType type) const;

private:
    std::vector<VendorEndpoint> vendors_;
    mutable std::mutex mutex_;
    
    VendorEndpoint* select_vendor();
};

#endif
