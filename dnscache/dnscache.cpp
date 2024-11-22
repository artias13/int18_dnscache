// dnscache.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

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
    std::cout << dnsCache->resolve("example.com") << std::endl;  // Выведет "192.168.1.1"

    return 0;
}


// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
