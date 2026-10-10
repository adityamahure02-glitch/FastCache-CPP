# FastCache-CPP: High-Throughput In-Memory LRU Cache Engine

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![Performance](https://img.shields.io/badge/Throughput-1.5M%2B%20ops%2Fsec-brightgreen.svg)]()
[![Latency](https://img.shields.io/badge/Latency-0.64%20%C2%B5s-brightgreen.svg)]()

FastCache-CPP is a zero-dependency, ultra-low-latency in-memory Key-Value Caching Engine implemented from scratch in modern C++. It marries an `std::unordered_map` with an explicit Doubly Linked List protected by Sentinel (dummy) nodes, delivering strict $O(1)$ operations with passive Time-To-Live (TTL) key expiration.

---

## 🏛️ System Architecture

+-------------------------------------------------------------+
      |             std::unordered_map<Key, Node<K,V>*>             |
      +-------------------------------------------------------------+
                     |                              |
                     v (O(1) Direct Lookup)         v (O(1) Direct Lookup)
[SENTINEL HEAD] <=> [Node: Key1] <=> [SENTINEL TAIL]
(MRU End)                    (Val + TTL)                     (LRU End)

## ⚡ Performance Micro-Benchmarks

Benchmarked on **Windows (MinGW GCC)** running 50,000 mixed Read/Write concurrent operations:

| Metric | Measured Value |
| :--- | :--- |
| **Total Operations** | 50,000 Ops |
| **Elapsed Time** | **31.99 ms** |
| **Average Latency** | **0.6398 microseconds / op** (Sub-microsecond) |
| **Peak Throughput** | **1,562,990 ops / second** (~1.56 Million ops/sec) |
| **Eviction Policy** | Strict $O(1)$ LRU with Sentinel Safety |
| **TTL Mechanism** | Passive Expiration on Key Hit |

---

## 🚀 Key Features

* **Generic Metaprogramming:** Implemented as `template <typename K, typename V>` supporting primitive types, pointers, and custom objects.
* **Passive TTL Eviction:** Auto-reclaims stale keys on read attempts without background worker overhead.
* **Zero Boundary Overhead:** Head and tail sentinel nodes eliminate pointer edge cases, preventing segfaults.
* **Zero Memory Leaks:** Explicit manual deallocation via class destructor and cache pruning.

---

## 🛠️ Build and Run

```bash
# Compile
g++ -O3 fastcache.cpp -o fastcache.exe

# Execute Benchmark Suite
./fastcache.exe