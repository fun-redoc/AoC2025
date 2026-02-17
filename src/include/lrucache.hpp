#pragma once
#include <unordered_map>
#include <list>
#include <iostream>
#include <optional>

template <typename K,
          typename V,
          typename KeyHash = std::hash<K>,              // Default hash function
          typename KeyEqual = std::equal_to<K>       // Default equality comparator>
         >
class LRUCache {
private:
    size_t capacity;
    std::list<std::pair<K, V>> lru_list;
    std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator, KeyHash, KeyEqual> cache_map;

public:
    LRUCache(int cap) : capacity(cap) {}
    LRUCache(int cap, const KeyHash& hash, const KeyEqual& equal) 
            //: capacity(cap), cache_map(10, hash, equal) {}
            : capacity(cap), cache_map(hash, equal) {}

    bool contains(K key) {
        auto it = cache_map.find(key);
        if (it == cache_map.end()) {
            return false;
        }
        // Move the accessed item to the front (most recently used)
        lru_list.splice(lru_list.begin(), lru_list, it->second);
        // Move the accessed item to the front (most recently used)
        lru_list.splice(lru_list.begin(), lru_list, it->second);
        return true;
    }

    std::optional<V> get(K key) {
        auto it = cache_map.find(key);
        if (it == cache_map.end()) {
            return std::nullopt;
        }
        // Move the accessed item to the front (most recently used)
        lru_list.splice(lru_list.begin(), lru_list, it->second);
        return it->second->second;
    }

    void put(K key, V value) {
        auto it = cache_map.find(key);

        // If key already exists, update and move to front
        if (it != cache_map.end()) {
            lru_list.erase(it->second);
            cache_map.erase(it);
        }
        // If cache is full, remove least recently used item
        else if (cache_map.size() >= capacity) {
            auto last = lru_list.back();
            cache_map.erase(last.first);
            lru_list.pop_back();
        }

        // Add new item to the front
        lru_list.push_front({key, value});
        cache_map[key] = lru_list.begin();
    }

    // Optional: Print cache contents for debugging
    void printCache() {
        for (const auto& item : lru_list) {
            std::cout << "Key: " << item.first 
                      << ", Value: " << item.second << std::endl;
        }
    }
};

/* Example usage
    LRUCache<int, std::string> cache(3);  // Cache with capacity 3

    cache.put(1, "one");
    cache.put(2, "two");
    cache.put(3, "three");

    try {
        std::cout << "Value for key 2: " << cache.get(2) << std::endl;
        
        cache.put(4, "four");  // This will evict the least recently used item (1)
        
        // This would throw an exception
        // std::cout << "Value for key 1: " << cache.get(1) << std::endl;
        
        cache.printCache();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
*/
