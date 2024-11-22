// dnscache.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.

#include <iostream>
#include <string>
#include <list>
#include <unordered_map>
#include <mutex>
#include <condition_variable>

class DNSCache {
private:
    size_t maxSize;
    std::list<std::pair<std::string, std::string>> orderList;
    std::unordered_map<std::string, std::list<std::pair<std::string, std::string>>::iterator> cacheMap;
    mutable std::mutex mtx;

public:
    explicit DNSCache(size_t max_size) : maxSize(max_size) {}

    void update(const std::string& name, const std::string& ip) {
        std::unique_lock<std::mutex> lock(mtx);
        auto it = cacheMap.find(name);
        if (it != cacheMap.end()) {
            // Обновляем существующую запись
            it->second->second = ip;
            orderList.splice(orderList.end(), orderList, it->second);
        }
        else {
            // Добавляем новую запись
            if (orderList.size() >= maxSize) {
                // Удаляем старейшую запись, если достигнут лимит
                cacheMap.erase(orderList.front().first);
                orderList.pop_front();
            }
            orderList.emplace_back(name, ip);
            cacheMap[name] = --orderList.end();
        }
    }

    std::string resolve(const std::string& name) {
        std::unique_lock<std::mutex> lock(mtx);
        auto it = cacheMap.find(name);
        if (it != cacheMap.end()) {
            // Перемещаем найденную запись в конец списка
            orderList.splice(orderList.end(), orderList, it->second);
            return it->second->second;
        }
        return "";
    }
};

// Реализация паттерна Singleton
class DNSCacheSingleton {
private:
    static DNSCache* instance;
    static std::mutex mtx;

    DNSCacheSingleton() {}
    ~DNSCacheSingleton() {}

    DNSCacheSingleton(const DNSCacheSingleton&) = delete;
    DNSCacheSingleton& operator=(const DNSCacheSingleton&) = delete;

public:
    static DNSCache* getInstance(size_t maxSize) {
        std::lock_guard<std::mutex> lock(mtx);
        if (!instance) {
            instance = new DNSCache(maxSize);
        }
        return instance;
    }
};

DNSCache* DNSCacheSingleton::instance = nullptr;
std::mutex DNSCacheSingleton::mtx;

// Использование
int main() {
    DNSCache* dnsCache = DNSCacheSingleton::getInstance(100);

    dnsCache->update("example.com", "192.168.1.1");
    dnsCache->update("google.com", "8.8.8.8");
    dnsCache->update("yandex.com", "77.77.7.3");
    dnsCache->update("cf.com", "1.1.1.1");
    std::cout << dnsCache->resolve("example.com") << std::endl;  // Выведет "192.168.1.1"
    std::cout << dnsCache->resolve("cf.com") << std::endl;  // Выведет "1.1.1.1"

    return 0;
}