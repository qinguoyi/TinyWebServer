// TopicManager.h
#pragma once
#include <unordered_map>
#include <unordered_set>
#include <shared_mutex>
#include <string>
#include <vector>

class TopicManager {
public:
    static TopicManager& getInstance() {
        static TopicManager instance;
        return instance;
    }

    // 处理客户端订阅
    void subscribe(int fd, const std::string& topic) {
        std::unique_lock<std::shared_mutex> lock(rw_mutex_);
        subscribers_[topic].insert(fd);
        client_topics_[fd].insert(topic);
    }

    // 处理客户端取消订阅/断开连接
    void unsubscribeAll(int fd) {
        std::unique_lock<std::shared_mutex> lock(rw_mutex_);
        auto it = client_topics_.find(fd);
        if (it != client_topics_.end()) {
            for (const auto& topic : it->second) {
                subscribers_[topic].erase(fd);
            }
            client_topics_.erase(it);
        }
    }

    // 获取需要推送的 FD 列表 (行情线程调用)
    std::vector<int> getSubscribers(const std::string& topic) {
        std::shared_lock<std::shared_mutex> lock(rw_mutex_);
        std::vector<int> fds;
        auto it = subscribers_.find(topic);
        if (it != subscribers_.end()) {
            fds.assign(it->second.begin(), it->second.end());
        }
        return fds;
    }

private:
    std::shared_mutex rw_mutex_;
    std::unordered_map<std::string, std::unordered_set<int>> subscribers_; // Topic -> FDs
    std::unordered_map<int, std::unordered_set<std::string>> client_topics_; // FD -> Topics (用于快速清理)
};
