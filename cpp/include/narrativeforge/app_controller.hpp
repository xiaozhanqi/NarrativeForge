#pragma once

#include "module_interface.hpp"
#include "core.hpp"
#include <memory>
#include <string>
#include <functional>
#include <future>
#include <atomic>
#include <mutex>

namespace narrativeforge {

struct AppConfig {
    std::string config_dir = "config";
    std::string data_dir = "data";
    std::string log_level = "info";
    bool enable_gui = true;
    bool enable_web = true;
    bool enable_cli = true;
    int web_port = 5000;
    std::string default_provider = "kimi";
    std::string default_model = "kimi-2.5";
};

struct RuntimeContext {
    std::string project_id;
    std::string session_id;
    std::string working_dir;
};

class AppController : public IModule {
public:
    static AppController& instance();
    
    // IModule interface
    bool initialize() override;
    bool start() override;
    bool stop() override;
    bool pause() override;
    bool resume() override;
    bool shutdown() override;
    ModuleStatus::Status get_status() const override;
    std::string get_name() const override { return "AppController"; }
    
    // Configuration
    bool load_config(const std::string& config_file = "config/app_config.json");
    bool save_config(const std::string& config_file = "config/app_config.json");
    AppConfig get_config() const { return config_; }
    void set_config(const AppConfig& config) { config_ = config; }
    
    // Module management
    bool register_module(ModuleType::Type type, std::shared_ptr<IModule> module);
    bool unregister_module(ModuleType::Type type);
    std::shared_ptr<IModule> get_module(ModuleType::Type type);
    bool has_module(ModuleType::Type type) const;
    
    // High-level operations
    std::future<NarrativeTensor> analyze_text(const std::string& text, const std::string& project_id = "default");
    std::future<std::string> generate_text(const NarrativeTensor& tensor, const std::string& mode = "continuation");
    
    // Runtime context
    RuntimeContext& context() { return context_; }
    const RuntimeContext& context() const { return context_; }
    
    // Error handling
    void set_error_callback(std::function<void(const std::string&)> callback);
    void report_error(const std::string& error);
    std::string get_last_error() const;
    
    // Progress reporting
    void set_progress_callback(std::function<void(int, const std::string&)> callback);
    void report_progress(int progress, const std::string& message);
    
private:
    AppController();
    ~AppController();
    AppController(const AppController&) = delete;
    AppController& operator=(const AppController&) = delete;
    
    class Impl;
    std::unique_ptr<Impl> pImpl_;
    
    AppConfig config_;
    RuntimeContext context_;
    std::atomic<ModuleStatus::Status> status_;
    std::function<void(const std::string&)> error_callback_;
    std::function<void(int, const std::string&)> progress_callback_;
    std::map<ModuleType::Type, std::shared_ptr<IModule>> modules_;
    std::string last_error_;
    std::mutex mutex_;
};

// Convenience function
inline AppController& app() {
    return AppController::instance();
}

} // namespace narrativeforge
