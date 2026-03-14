#include "narrativeforge/api_router.hpp"
#include "narrativeforge/api_key_manager.hpp"
#include <mutex>

namespace narrativeforge {

class APIKeyManager::Impl {
public:
    Impl() : db_("api_keys.txt") {}
    
    bool setup_api_key(const std::string& provider, const std::string& api_key, const std::string& model) {
        return db_.setup_api_key(provider, api_key, model);
    }
    
    std::string get_api_key(const std::string& provider) const {
        auto key = db_.get_api_key(provider);
        return key.has_value() ? *key : "";
    }
    
    bool has_api_key(const std::string& provider) const {
        return db_.has_api_key(provider);
    }
    
    std::vector<std::pair<std::string, std::string>> get_all_providers() const {
        std::vector<std::pair<std::string, std::string>> result;
        auto providers = db_.get_all_providers();
        for (const auto& info : providers) {
            result.emplace_back(info.provider, info.model);
        }
        return result;
    }
    
    bool delete_api_key(const std::string& provider) {
        return db_.delete_api_key(provider);
    }
    
    bool is_first_run() const {
        return db_.is_first_run();
    }
    
private:
    APIKeyDatabase db_;
};

APIKeyManager& APIKeyManager::instance() {
    static APIKeyManager instance;
    return instance;
}

APIKeyManager::APIKeyManager() : pImpl_(std::make_unique<Impl>()) {}

APIKeyManager::~APIKeyManager() = default;

bool APIKeyManager::setup_api_key(const std::string& provider, 
                                   const std::string& api_key,
                                   const std::string& model) {
    return pImpl_->setup_api_key(provider, api_key, model);
}

std::string APIKeyManager::get_api_key(const std::string& provider) const {
    return pImpl_->get_api_key(provider);
}

bool APIKeyManager::has_api_key(const std::string& provider) const {
    return pImpl_->has_api_key(provider);
}

std::vector<std::pair<std::string, std::string>> APIKeyManager::get_all_providers() const {
    return pImpl_->get_all_providers();
}

bool APIKeyManager::delete_api_key(const std::string& provider) {
    return pImpl_->delete_api_key(provider);
}

bool APIKeyManager::is_first_run() const {
    return pImpl_->is_first_run();
}

} // namespace narrativeforge
