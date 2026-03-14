#ifndef NARRATIVEFORGE_UNIFIED_CONTROLLER_HPP
#define NARRATIVEFORGE_UNIFIED_CONTROLLER_HPP

#include "narrativeforge/core.hpp"
#include "narrativeforge/api_router.hpp"
#include "narrativeforge/api_key_manager.hpp"
#include "narrativeforge/module_interface.hpp"
#include "narrativeforge/app_controller.hpp"
#include <string>
#include <memory>
#include <functional>
#include <future>
#include <map>
#include <mutex>

namespace narrativeforge {

class UnifiedController {
public:
    static UnifiedController& instance();
    
    ~UnifiedController();
    
    bool initialize();
    bool start();
    bool stop();
    bool shutdown();
    
    ModuleStatus::Status get_status() const;
    
    // Core functionality
    std::future<NarrativeTensor> analyze_text(const std::string& text, const std::string& project_id = "default");
    std::future<std::string> generate_text(const NarrativeTensor& tensor, const std::string& mode = "continuation");
    std::future<std::string> run_full_pipeline(const std::string& text, const std::string& mode = "continuation");
    
    // Configuration
    bool load_config(const std::string& config_file);
    bool save_config(const std::string& config_file);
    const AppConfig& get_config() const;
    
    // API key management
    bool setup_api_key(const std::string& provider, const std::string& key, const std::string& model);
    bool is_api_key_set(const std::string& provider) const;
    
    // Module management
    bool register_module(ModuleType::Type type, std::shared_ptr<IModule> module);
    bool unregister_module(ModuleType::Type type);
    std::shared_ptr<IModule> get_module(ModuleType::Type type);
    std::vector<ModuleInfo> get_all_modules() const;
    
    // Callbacks
    void set_error_callback(std::function<void(const std::string&)> callback);
    void set_progress_callback(std::function<void(int, const std::string&)> callback);
    
    // Utility
    std::string get_last_error() const;
    std::string generate_session_id() const;
    
private:
    UnifiedController();
    
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    
    ModuleStatus::Status status_;
    std::string last_error_;
    AppConfig config_;
    std::map<ModuleType::Type, std::shared_ptr<IModule>> modules_;
    std::function<void(const std::string&)> error_callback_;
    std::function<void(int, const std::string&)> progress_callback_;
    std::mutex mutex_;
    
    // Context
    struct Context {
        std::string project_id;
        std::string session_id;
        std::string working_dir;
    } context_;
    
    bool initialize_modules();
    void report_error(const std::string& error);
    void report_progress(int progress, const std::string& message);
};

} // namespace narrativeforge

#endif // NARRATIVEFORGE_UNIFIED_CONTROLLER_HPP
