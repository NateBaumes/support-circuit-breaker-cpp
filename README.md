# ⚡ High-Performance Support Circuit Breaker (C++)

Production-ready support ticket router with circuit breakers, rate limiting, and vendor failover. Handles 100K+ requests/sec with sub-millisecond latency.

## 🎯 What This Demonstrates

| Capability | Implementation |
|------------|----------------|
| **High Performance** | C++20, lock-free queues, zero-copy parsing |
| **Circuit Breakers** | 3-state breaker with recovery timeouts |
| **Rate Limiting** | Token bucket at 10K req/sec per tenant |
| **Vendor Failover** | Primary → Secondary → Fallback routing |
| **GenAI Integration** | Async HTTP to OpenAI/Anthropic |
| **Memory Safety** | RAII, smart pointers, arena allocators |

## 🏗️ Architecture
