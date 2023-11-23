//
// Created by Steel_Shadow on 2023/9/24.
//

#ifndef COMPILER_LINKEDHASHMAP_H
#define COMPILER_LINKEDHASHMAP_H

#include <list>
#include <unordered_map>
#include <stdexcept>

template<typename K, typename V>
class LinkedHashMap {
public:
    void put(const K &key, const V &value) {
        auto it = key_to_iterator.find(key);
        if (it != key_to_iterator.end()) {
            // Key already exists, update the value
            it->second->second = value;
        } else {
            // Key doesn't exist, add a new entry
            data.push_back(std::make_pair(key, value));
            auto list_iterator = std::prev(data.end());
            key_to_iterator[key] = list_iterator;
        }
    }

    bool containsKey(const K &key) const {
        // ReSharper disable once CppUseAssociativeContains
        return key_to_iterator.find(key) != key_to_iterator.end();
    }

    V get(const K &key) const {
        auto it = key_to_iterator.find(key);
        if (it != key_to_iterator.end()) {
            return it->second->second;
        } else {
            throw std::out_of_range("Key not found");
        }
    }

    void remove(const K &key) {
        auto it = key_to_iterator.find(key);
        if (it != key_to_iterator.end()) {
            data.erase(it->second);
            key_to_iterator.erase(it);
        }
    }

    void replace(const K &key, const V &value) {
        auto it = key_to_iterator.find(key);
        if (it != key_to_iterator.end()) {
            it->second->second = value;
        }
    }

    // iterator for std::list
    typename std::list<std::pair<K, V>>::iterator begin() {
        return data.begin();
    }

    typename std::list<std::pair<K, V>>::iterator end() {
        return data.end();
    }

private:
    std::list<std::pair<K, V>> data;
    std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> key_to_iterator;
};

#endif //COMPILER_LINKEDHASHMAP_H
