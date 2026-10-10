#pragma once
#include <unordered_map>
#include <chrono>

// 1. Generic Node with TTL Metadata
template <typename K, typename V>
struct Node {
    K key;
    V value;
    std::chrono::steady_clock::time_point expiryTime;
    bool hasTTL;
    Node* prev;
    Node* next;

    Node(K k, V v, int ttlSeconds = 0) 
        : key(k), value(v), prev(nullptr), next(nullptr) {
        if (ttlSeconds > 0) {
            hasTTL = true;
            expiryTime = std::chrono::steady_clock::now() + std::chrono::seconds(ttlSeconds);
        } else {
            hasTTL = false;
        }
    }
};

// 2. Production FastCache Engine
template <typename K, typename V>
class FastCache {
private:
    int capacity;
    std::unordered_map<K, Node<K, V>*> cacheMap;
    Node<K, V>* head;
    Node<K, V>* tail;

    void removeNode(Node<K, V>* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void addNode(Node<K, V>* node) {
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
    }

    void moveToHead(Node<K, V>* node) {
        removeNode(node);
        addNode(node);
    }

    void deleteNode(Node<K, V>* node) {
        removeNode(node);
        cacheMap.erase(node->key);
        delete node;
    }

public:
    FastCache(int cap) : capacity(cap) {
        head = new Node<K, V>(K(), V());
        tail = new Node<K, V>(K(), V());
        head->next = tail;
        tail->prev = head;
    }

    ~FastCache() {
        Node<K, V>* curr = head;
        while (curr != nullptr) {
            Node<K, V>* nextNode = curr->next;
            delete curr;
            curr = nextNode;
        }
    }

    void put(const K& key, const V& value, int ttlSeconds = 0) {
        if (cacheMap.find(key) != cacheMap.end()) {
            Node<K, V>* node = cacheMap[key];
            node->value = value;
            if (ttlSeconds > 0) {
                node->hasTTL = true;
                node->expiryTime = std::chrono::steady_clock::now() + std::chrono::seconds(ttlSeconds);
            } else {
                node->hasTTL = false;
            }
            moveToHead(node);
            return;
        }

        Node<K, V>* newNode = new Node<K, V>(key, value, ttlSeconds);
        cacheMap[key] = newNode;
        addNode(newNode);

        if (cacheMap.size() > (size_t)capacity) {
            Node<K, V>* lru = tail->prev;
            deleteNode(lru);
        }
    }

    bool get(const K& key, V& outValue) {
        if (cacheMap.find(key) == cacheMap.end()) {
            return false;
        }

        Node<K, V>* node = cacheMap[key];

        // Passive TTL Expiration check
        if (node->hasTTL && std::chrono::steady_clock::now() > node->expiryTime) {
            deleteNode(node);
            return false;
        }

        moveToHead(node);
        outValue = node->value;
        return true;
    }
};