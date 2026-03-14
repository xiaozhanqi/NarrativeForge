#pragma once

#include <string>
#include <memory>
#include <functional>
#include <future>
#include <map>
#include <vector>
#include <nlohmann/json.hpp>

namespace narrativeforge {

namespace ModuleType {
    enum Type {
        ANALYZER,
        GENERATOR,
        API_ROUTER,
        API_KEY_MANAGER,
        DATABASE,
        DOMAIN_ADAPTER,
        GUI,
        WEB_SERVER,
        CLI
    };
}

namespace ModuleStatus {
    enum Status {
        UNINITIALIZED,
        INITIALIZING,
        READY,
        RUNNING,
        PAUSED,
        ERROR,
        SHUTDOWN
    };
}

struct ModuleInfo {
    ModuleType::Type type;
    std::string name;
    std::string version;
    ModuleStatus::Status status;
    std::string description;
};

class IModule {
public:
    virtual ~IModule() = default;
    
    // Lifecycle
    virtual bool initialize() = 0;
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual bool pause() = 0;
    virtual bool resume() = 0;
    virtual bool shutdown() = 0;
    
    // Status
    virtual ModuleStatus::Status get_status() const = 0;
    virtual std::string get_name() const = 0;
    
    // Configuration
    virtual bool load_config(const nlohmann::json& config) = 0;
    virtual nlohmann::json save_config() const = 0;
    
    // Error handling
    virtual void set_error_callback(std::function<void(const std::string&)> callback) = 0;
    virtual std::string get_last_error() const = 0;
    
    // Progress reporting
    virtual void set_progress_callback(std::function<void(int, const std::string&)> callback) = 0;
};

class ModuleManager {
public:
    static ModuleManager& instance();
    
    bool register_module(ModuleType::Type type, std::shared_ptr<IModule> module);
    bool unregister_module(ModuleType::Type type);
    std::shared_ptr<IModule> get_module(ModuleType::Type type);
    bool has_module(ModuleType::Type type) const;
    
    std::vector<ModuleInfo> get_all_modules() const;
    
    bool initialize_all();
    bool start_all();
    bool stop_all();
    bool shutdown_all();
    
    void set_global_error_callback(std::function<void(ModuleType::Type, const std::string&)> callback);
    
private:
    ModuleManager() = default;
    ~ModuleManager() = default;
    ModuleManager(const ModuleManager&) = delete;
    ModuleManager& operator=(const ModuleManager&) = delete;
    
    std::map<ModuleType::Type, std::shared_ptr<IModule>> modules_;
    std::function<void(ModuleType::Type, const std::string&)> error_callback_;
};

} // namespace narrativeforge
