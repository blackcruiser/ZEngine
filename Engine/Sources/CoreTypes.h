#pragma once

#include <memory>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>


namespace ZE {

template<typename T>
using TPtr = std::shared_ptr<T>;

template<typename T>
using TWeakPtr = std::weak_ptr<T>;

template<typename T>
using TPtrArr = std::vector<std::shared_ptr<T>>;

template<typename T>
using TPtrList = std::list<std::shared_ptr<T>>;

template<typename Key, typename Value>
using TPtrMap = std::map<Key, std::shared_ptr<Value>>;

template<typename Key, typename Value>
using TPtrUnorderedMap = std::unordered_map<Key, std::shared_ptr<Value>>;

template<typename value>
using TPtrSet = std::set<std::shared_ptr<value>>;

template<typename value>
using TPtrUnorderedSet = std::unordered_set<std::shared_ptr<value>>;

}