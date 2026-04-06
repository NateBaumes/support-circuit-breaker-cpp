#include "vendor_router.h"
#include <algorithm>
#include <iostream>

VendorRouter::VendorRouter() {}

void VendorRouter::add_vendor(VendorType type, const std::string& url,
                              const std::string& api_key, int priority) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    VendorEndpoint vendor;
    vendor.type = type;
    vendor.url = url;
    vendor.api_key = api_key;
    vendor.priority = priority;
    vendor.circuit_breaker = std::make_unique<CircuitBreaker>(3, 60, 2);
    
    vendors_.push_back(std::move(vendor));
    
    // Sort by priority
    std::sort(vendors_.begin(), vendors_.end(),
              [](const auto& a, const auto& b) {
                  return a.priority < b.priority;
              });
}

std::string VendorRouter::route_request(const std::string& ticket_text) {
    auto* vendor = select_vendor();
    
    if (!vendor) {
        return "Error: No healthy vendors available";
    }
    
    // Simulate request to vendor
    // In production, this would make actual HTTP call
    
    if (!vendor->circuit_breaker->allow_request()) {
        // Try next vendor
        vendor = select_vendor();
        if (!vendor) {
            return "Error: All vendors unavailable";
        }
    }
    
    // Simulate successful classification
    vendor->circuit_breaker->record_success();
    
    return "Processed by vendor: " + std::to_string(static_cast<int>(vendor->type));
}

bool VendorRouter::is_vendor_healthy(VendorType type) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& vendor : vendors_) {
        if (vendor.type == type) {
            return vendor.circuit_breaker->get_state() != CircuitState::OPEN;
        }
    }
    return false;
}

VendorEndpoint* VendorRouter::select_vendor() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& vendor : vendors_) {
        if (vendor.circuit_breaker->allow_request()) {
            return &vendor;
        }
    }
    
    return nullptr;
}
