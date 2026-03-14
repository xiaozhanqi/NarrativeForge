#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <future>
#include <functional>
#include <mutex>
#include <semaphore>
#include <nlohmann/json.hpp>

namespace narrativeforge {

struct APIResponse {
    bool success;
    std::string content;
    std::string error;
    int status_code;
};

struct APITask {
    std::string id;
    std::string provider;
    std::string model;
    std::vector<std::map<std::string, std::string>> messages;
    std::map<std::string, double> parameters;
};

class APIKeyManager {
public:
    static APIKeyManager& instance();
    
    bool setup_api_key(const std::string& provider, 
                       const std::string& api_key,
                       const std::string& model = "");
    
    std::string get_api_key(const std::string& provider) const;
    bool has_api_key(const std::string& provider) const;
    
    std::vector<std::pair<std::string, std::string>> get_all_providers() const;
    bool delete_api_key(const std::string& provider);
    
    bool is_first_run() const;
    
private:
    APIKeyManager();
    ~APIKeyManager();
    APIKeyManager(const APIKeyManager&) = delete;
    APIKeyManager& operator=(const APIKeyManager&) = delete;
    
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

class UniversalAPIRouter {
public:
    UniversalAPIRouter();
    ~UniversalAPIRouter();
    
    std::future<APIResponse> call_single(
        const std::string& provider,
        const std::string& model,
        const std::vector<std::map<std::string, std::string>>& messages,
        const std::map<std::string, double>& parameters = {}
    );
    
    std::future<std::vector<APIResponse>> batch_call(
        const std::vector<APITask>& tasks,
        int max_concurrent = 4,
        std::function<void(int, int)> progress_callback = nullptr
    );
    
    bool check_api_key(const std::string& provider) const;
    std::map<std::string, bool> health_check() const;
    void reset_health(const std::string& provider = "");
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace narrativeforge
