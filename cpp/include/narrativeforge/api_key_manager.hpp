#pragma once

#include <string>
#include <vector>
#include <optional>
#include <memory>
#include <map>

namespace narrativeforge {

struct APIKeyInfo {
    std::string provider;
    std::string model;
    std::string created_at;
    std::string updated_at;
};

class APIKeyDatabase {
public:
    APIKeyDatabase(const std::string& db_path = "data/api_keys.db");
    ~APIKeyDatabase();
    
    bool setup_api_key(
        const std::string& provider,
        const std::string& api_key,
        const std::string& model = ""
    );
    
    std::optional<std::string> get_api_key(const std::string& provider) const;
    bool has_api_key(const std::string& provider) const;
    
    std::vector<APIKeyInfo> get_all_providers() const;
    bool delete_api_key(const std::string& provider);
    
    bool is_first_run() const;
    bool is_machine_bound() const;
    
    std::string get_machine_id() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace narrativeforge
