#include "narrativeforge/api_key_manager.hpp"
#include <sstream>
#include <iomanip>
#include <random>
#include <chrono>
#include <fstream>

namespace narrativeforge {

class APIKeyDatabase::Impl {
public:
    Impl(const std::string& db_path) : db_path_(db_path) {
        machine_id_ = generate_machine_id();
        load_from_file();
    }
    
    ~Impl() {
        save_to_file();
    }
    
    void load_from_file() {
        std::ifstream file(db_path_);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                if (line.empty()) continue;
                size_t pos1 = line.find('|');
                size_t pos2 = line.find('|', pos1 + 1);
                size_t pos3 = line.find('|', pos2 + 1);
                if (pos1 != std::string::npos && pos2 != std::string::npos) {
                    std::string provider = line.substr(0, pos1);
                    std::string key = line.substr(pos1 + 1, pos2 - pos1 - 1);
                    std::string model = (pos3 != std::string::npos) ? 
                        line.substr(pos2 + 1, pos3 - pos2 - 1) : "";
                    api_keys_[provider] = {key, model};
                }
            }
        }
    }
    
    void save_to_file() {
        std::ofstream file(db_path_);
        if (file.is_open()) {
            for (const auto& [provider, info] : api_keys_) {
                file << provider << "|" << info.key << "|" << info.model << "|\n";
            }
        }
    }
    
    bool setup_api_key(const std::string& provider, const std::string& api_key, const std::string& model) {
        api_keys_[provider] = {api_key, model};
        save_to_file();
        return true;
    }
    
    std::optional<std::string> get_api_key(const std::string& provider) const {
        auto it = api_keys_.find(provider);
        if (it != api_keys_.end()) {
            return it->second.key;
        }
        return std::nullopt;
    }
    
    bool has_api_key(const std::string& provider) const {
        return api_keys_.count(provider) > 0;
    }
    
    std::vector<APIKeyInfo> get_all_providers() const {
        std::vector<APIKeyInfo> result;
        for (const auto& [provider, info] : api_keys_) {
            APIKeyInfo key_info;
            key_info.provider = provider;
            key_info.model = info.model;
            key_info.created_at = "2024-01-01 00:00:00";
            key_info.updated_at = "2024-01-01 00:00:00";
            result.push_back(key_info);
        }
        return result;
    }
    
    bool delete_api_key(const std::string& provider) {
        auto it = api_keys_.find(provider);
        if (it != api_keys_.end()) {
            api_keys_.erase(it);
            save_to_file();
            return true;
        }
        return false;
    }
    
    bool is_machine_bound() const {
        return true;
    }
    
    std::string get_machine_id() const {
        return machine_id_;
    }
    
    std::string generate_machine_id() const {
        std::stringstream ss;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);
        
        for (int i = 0; i < 32; ++i) {
            ss << std::hex << dis(gen);
        }
        return ss.str();
    }
    
private:
    struct KeyInfo {
        std::string key;
        std::string model;
    };
    
    std::string db_path_;
    std::map<std::string, KeyInfo> api_keys_;
    std::string machine_id_;
};

APIKeyDatabase::APIKeyDatabase(const std::string& db_path)
    : pImpl_(std::make_unique<Impl>(db_path)) {}

APIKeyDatabase::~APIKeyDatabase() = default;

bool APIKeyDatabase::setup_api_key(const std::string& provider, 
                                   const std::string& api_key,
                                   const std::string& model) {
    return pImpl_->setup_api_key(provider, api_key, model);
}

std::optional<std::string> APIKeyDatabase::get_api_key(const std::string& provider) const {
    return pImpl_->get_api_key(provider);
}

bool APIKeyDatabase::has_api_key(const std::string& provider) const {
    return pImpl_->has_api_key(provider);
}

std::vector<APIKeyInfo> APIKeyDatabase::get_all_providers() const {
    return pImpl_->get_all_providers();
}

bool APIKeyDatabase::delete_api_key(const std::string& provider) {
    return pImpl_->delete_api_key(provider);
}

bool APIKeyDatabase::is_first_run() const {
    return !pImpl_->is_machine_bound();
}

bool APIKeyDatabase::is_machine_bound() const {
    return pImpl_->is_machine_bound();
}

std::string APIKeyDatabase::get_machine_id() const {
    return pImpl_->get_machine_id();
}

} // namespace narrativeforge
