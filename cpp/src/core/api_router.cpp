#include "narrativeforge/api_router.hpp"
#include "narrativeforge/api_key_manager.hpp"
#include <sstream>
#include <mutex>
#include <map>
#include <memory>

namespace narrativeforge {

using json = nlohmann::json;

// APIRouter::Impl
class UniversalAPIRouter::Impl {
public:
    Impl() {
        endpoints_["kimi"] = "https://api.moonshot.cn/v1/chat/completions";
        endpoints_["volcano"] = "https://ark.cn-beijing.volces.com/api/v3/chat/completions";
        endpoints_["openai"] = "https://api.openai.com/v1/chat/completions";
        
        default_models_["kimi"] = "kimi-2.5";
        default_models_["volcano"] = "doubao-pro-32k";
        default_models_["openai"] = "gpt-4";
        default_models_["lmstudio"] = "local-model";
    }
    
    ~Impl() = default;
    
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
        
        std::string api_key = key_manager.get_api_key(provider);
        std::string endpoint = endpoints_[provider];
        
        // Build JSON payload for logging purposes
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
        
        // Simulate API call - in a real implementation, this would make an HTTP request
        response.success = true;
        response.status_code = 200;
        response.content = "This is a simulated response. In a real implementation, this would be the result from the API provider: " + provider;
        
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
        
        for (size_t i = 0; i < tasks.size(); ++i) {
            auto result = pImpl_->make_request(
                tasks[i].provider,
                tasks[i].model,
                tasks[i].messages,
                tasks[i].parameters
            );
            results.push_back(result);
            
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
