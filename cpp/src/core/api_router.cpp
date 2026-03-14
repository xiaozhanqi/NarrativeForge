#include "narrativeforge/api_router.hpp"
#include "narrativeforge/api_key_manager.hpp"
#include <curl/curl.h>
#include <sstream>
#include <mutex>
#include <map>
#include <semaphore>
#include <memory>

namespace narrativeforge {

// Curl callback
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Rate Limiter
class RateLimiter {
public:
    explicit RateLimiter(int max_calls) 
        : semaphore_(std::make_unique<std::counting_semaphore<>>(max_calls)) {}
    
    void acquire() { semaphore_->acquire(); }
    void release() { semaphore_->release(); }
    
private:
    std::unique_ptr<std::counting_semaphore<>> semaphore_;
};

// APIRouter::Impl
class UniversalAPIRouter::Impl {
public:
    Impl() {
        curl_global_init(CURL_GLOBAL_ALL);
        
        rate_limiters_["kimi"] = std::make_unique<RateLimiter>(60);
        rate_limiters_["volcano"] = std::make_unique<RateLimiter>(120);
        rate_limiters_["lmstudio"] = std::make_unique<RateLimiter>(1000);
        rate_limiters_["h200"] = std::make_unique<RateLimiter>(100);
        rate_limiters_["openai"] = std::make_unique<RateLimiter>(100);
        
        endpoints_["kimi"] = "https://api.moonshot.cn/v1/chat/completions";
        endpoints_["volcano"] = "https://ark.cn-beijing.volces.com/api/v3/chat/completions";
        endpoints_["openai"] = "https://api.openai.com/v1/chat/completions";
        
        default_models_["kimi"] = "kimi-2.5";
        default_models_["volcano"] = "doubao-pro-32k";
        default_models_["openai"] = "gpt-4";
        default_models_["lmstudio"] = "local-model";
    }
    
    ~Impl() {
        curl_global_cleanup();
    }
    
    APIResponse make_request(
        const std::string& provider,
        const std::string& model,
        const std::vector<std::map<std::string, std::string>>& messages,
        const std::map<std::string, double>& parameters
    ) {
        APIResponse response;
        
        // Check API key
        auto& key_manager = APIKeyManager::instance();
        if (!key_manager.has_api_key(provider)) {
            response.success = false;
            response.error = "No API key configured for provider: " + provider;
            return response;
        }
        
        std::string api_key = key_manager.get_api_key(provider).value_or("");
        std::string endpoint = endpoints_[provider];
        
        // Build JSON payload
        json payload;
        payload["model"] = model.empty() ? default_models_[provider] : model;
        
        json msgs = json::array();
        for (const auto& msg : messages) {
            json m;
            for (const auto& [key, value] : msg) {
                m[key] = value;
            }
            msgs.push_back(m);
        }
        payload["messages"] = msgs;
        
        // Add parameters
        if (parameters.count("temperature")) {
            payload["temperature"] = parameters.at("temperature");
        }
        if (parameters.count("max_tokens")) {
            payload["max_tokens"] = parameters.at("max_tokens");
        }
        if (parameters.count("top_p")) {
            payload["top_p"] = parameters.at("top_p");
        }
        
        // Rate limiting
        auto& limiter = rate_limiters_[provider];
        limiter->acquire();
        
        // Make HTTP request
        CURL* curl = curl_easy_init();
        if (curl) {
            struct curl_slist* headers = nullptr;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            
            std::string auth_header = "Authorization: Bearer " + api_key;
            headers = curl_slist_append(headers, auth_header.c_str());
            
            std::string json_str = payload.dump();
            std::string read_buffer;
            
            curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120L);
            
            CURLcode res = curl_easy_perform(curl);
            
            if (res == CURLE_OK) {
                long http_code = 0;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
                response.status_code = static_cast<int>(http_code);
                
                if (http_code == 200) {
                    try {
                        json result = json::parse(read_buffer);
                        response.content = result["choices"][0]["message"]["content"];
                        response.success = true;
                    } catch (const std::exception& e) {
                        response.success = false;
                        response.error = "Failed to parse response: " + std::string(e.what());
                    }
                } else {
                    response.success = false;
                    response.error = "HTTP error: " + std::to_string(http_code);
                }
            } else {
                response.success = false;
                response.error = "CURL error: " + std::string(curl_easy_strerror(res));
            }
            
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        }
        
        limiter->release();
        return response;
    }
    
    bool check_api_key(const std::string& provider) const {
        return APIKeyManager::instance().has_api_key(provider);
    }
    
    std::map<std::string, bool> health_check() const {
        return health_status_;
    }
    
    void reset_health(const std::string& provider) {
        if (provider.empty()) {
            for (auto& [k, v] : health_status_) {
                v = true;
            }
        } else {
            health_status_[provider] = true;
        }
    }
    
private:
    std::map<std::string, std::unique_ptr<RateLimiter>> rate_limiters_;
    std::map<std::string, std::string> endpoints_;
    std::map<std::string, std::string> default_models_;
    std::map<std::string, bool> health_status_;
};

UniversalAPIRouter::UniversalAPIRouter() 
    : pImpl_(std::make_unique<Impl>()) {}

UniversalAPIRouter::~UniversalAPIRouter() = default;

std::future<APIResponse> UniversalAPIRouter::call_single(
    const std::string& provider,
    const std::string& model,
    const std::vector<std::map<std::string, std::string>>& messages,
    const std::map<std::string, double>& parameters
) {
    return std::async(std::launch::async, [this, provider, model, messages, parameters]() {
        return pImpl_->make_request(provider, model, messages, parameters);
    });
}

std::future<std::vector<APIResponse>> UniversalAPIRouter::batch_call(
    const std::vector<APITask>& tasks,
    int max_concurrent,
    std::function<void(int, int)> progress_callback
) {
    return std::async(std::launch::async, [this, tasks, max_concurrent, progress_callback]() {
        std::vector<APIResponse> results;
        results.reserve(tasks.size());
        
        std::counting_semaphore<> sem(max_concurrent);
        std::vector<std::future<APIResponse>> futures;
        
        for (size_t i = 0; i < tasks.size(); ++i) {
            futures.push_back(std::async(std::launch::async, [this, &sem, &tasks, i]() {
                sem.acquire();
                auto result = pImpl_->make_request(
                    tasks[i].provider,
                    tasks[i].model,
                    tasks[i].messages,
                    tasks[i].parameters
                );
                sem.release();
                return result;
            }));
        }
        
        for (size_t i = 0; i < futures.size(); ++i) {
            results.push_back(futures[i].get());
            if (progress_callback) {
                progress_callback(static_cast<int>(i + 1), static_cast<int>(tasks.size()));
            }
        }
        
        return results;
    });
}

bool UniversalAPIRouter::check_api_key(const std::string& provider) const {
    return pImpl_->check_api_key(provider);
}

std::map<std::string, bool> UniversalAPIRouter::health_check() const {
    return pImpl_->health_check();
}

void UniversalAPIRouter::reset_health(const std::string& provider) {
    pImpl_->reset_health(provider);
}

} // namespace narrativeforge
