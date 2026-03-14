#include "narrativeforge/unified_controller.hpp"
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <random>
#include <filesystem>

namespace narrativeforge {

class UnifiedController::Impl {
public:
    Impl() = default;
};

UnifiedController& UnifiedController::instance() {
    static UnifiedController instance;
    return instance;
}

UnifiedController::UnifiedController() 
    : status_(ModuleStatus::UNINITIALIZED),
      pImpl_(std::make_unique<Impl>()) {
    context_.project_id = "default";
    context_.session_id = generate_session_id();
    context_.working_dir = std::filesystem::current_path().string();
    
    // Default configuration
    config_.config_dir = "config";
    config_.data_dir = "data";
    config_.log_level = "info";
    config_.enable_gui = false;
    config_.enable_web = false;
    config_.enable_cli = true;
    config_.web_port = 8080;
    config_.default_provider = "kimi";
    config_.default_model = "kimi-2.5";
}

UnifiedController::~UnifiedController() {
    shutdown();
}

bool UnifiedController::initialize() {
    if (status_ != ModuleStatus::UNINITIALIZED) {
        return false;
    }
    
    status_ = ModuleStatus::INITIALIZING;
    
    try {
        // Create data directory
        std::filesystem::create_directories(config_.data_dir);
        
        status_ = ModuleStatus::READY;
        return true;
    } catch (const std::exception& e) {
        last_error_ = e.what();
        status_ = ModuleStatus::ERROR;
        return false;
    }
}

bool UnifiedController::start() {
    if (status_ != ModuleStatus::READY) {
        return false;
    }
    
    status_ = ModuleStatus::RUNNING;
    return true;
}

bool UnifiedController::stop() {
    if (status_ != ModuleStatus::RUNNING) {
        return false;
    }
    
    status_ = ModuleStatus::READY;
    return true;
}

bool UnifiedController::shutdown() {
    status_ = ModuleStatus::SHUTDOWN;
    return true;
}

ModuleStatus::Status UnifiedController::get_status() const {
    return status_;
}

std::future<NarrativeTensor> UnifiedController::analyze_text(const std::string& text, const std::string& project_id) {
    return std::async(std::launch::async, [this, text, project_id]() {
        CanonAnalysisEngine analyzer(
            "config/analyzer_schema.json",
            project_id
        );
        
        analyzer.set_progress_callback([this](int progress, const std::string& message) {
            report_progress(progress, message);
        });
        
        auto future = analyzer.run_analysis_pipeline(text);
        return future.get();
    });
}

std::future<std::string> UnifiedController::generate_text(const NarrativeTensor& tensor, const std::string& mode) {
    return std::async(std::launch::async, [this, tensor, mode]() {
        NarrativeGenerationEngine generator(
            "config/generator_schema.json",
            context_.project_id,
            tensor
        );
        
        generator.set_generation_mode(mode);
        generator.set_progress_callback([this](int progress, const std::string& message) {
            report_progress(progress, message);
        });
        
        auto future = generator.run_generation_pipeline();
        return future.get();
    });
}

std::future<std::string> UnifiedController::run_full_pipeline(const std::string& text, const std::string& mode) {
    return std::async(std::launch::async, [this, text, mode]() {
        // First analyze the text
        auto analyze_future = analyze_text(text);
        NarrativeTensor tensor = analyze_future.get();
        
        // Then generate text
        auto generate_future = generate_text(tensor, mode);
        return generate_future.get();
    });
}

bool UnifiedController::load_config(const std::string& config_file) {
    try {
        std::ifstream file(config_file);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json j;
        file >> j;
        
        if (j.contains("config_dir")) config_.config_dir = j["config_dir"];
        if (j.contains("data_dir")) config_.data_dir = j["data_dir"];
        if (j.contains("log_level")) config_.log_level = j["log_level"];
        if (j.contains("enable_gui")) config_.enable_gui = j["enable_gui"];
        if (j.contains("enable_web")) config_.enable_web = j["enable_web"];
        if (j.contains("enable_cli")) config_.enable_cli = j["enable_cli"];
        if (j.contains("web_port")) config_.web_port = j["web_port"];
        if (j.contains("default_provider")) config_.default_provider = j["default_provider"];
        if (j.contains("default_model")) config_.default_model = j["default_model"];
        
        return true;
    } catch (const std::exception& e) {
        last_error_ = e.what();
        return false;
    }
}

bool UnifiedController::save_config(const std::string& config_file) {
    try {
        nlohmann::json j;
        j["config_dir"] = config_.config_dir;
        j["data_dir"] = config_.data_dir;
        j["log_level"] = config_.log_level;
        j["enable_gui"] = config_.enable_gui;
        j["enable_web"] = config_.enable_web;
        j["enable_cli"] = config_.enable_cli;
        j["web_port"] = config_.web_port;
        j["default_provider"] = config_.default_provider;
        j["default_model"] = config_.default_model;
        
        std::ofstream file(config_file);
        file << j.dump(2);
        
        return true;
    } catch (const std::exception& e) {
        last_error_ = e.what();
        return false;
    }
}

const AppConfig& UnifiedController::get_config() const {
    return config_;
}

bool UnifiedController::setup_api_key(const std::string& provider, const std::string& key, const std::string& model) {
    APIKeyDatabase db;
    return db.setup_api_key(provider, key, model);
}

bool UnifiedController::is_api_key_set(const std::string& provider) const {
    APIKeyDatabase db;
    return db.has_api_key(provider);
}

bool UnifiedController::register_module(ModuleType::Type type, std::shared_ptr<IModule> module) {
    std::lock_guard<std::mutex> lock(mutex_);
    modules_[type] = module;
    return true;
}

bool UnifiedController::unregister_module(ModuleType::Type type) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = modules_.find(type);
    if (it != modules_.end()) {
        it->second->shutdown();
        modules_.erase(it);
        return true;
    }
    return false;
}

std::shared_ptr<IModule> UnifiedController::get_module(ModuleType::Type type) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = modules_.find(type);
    return it != modules_.end() ? it->second : nullptr;
}

std::vector<ModuleInfo> UnifiedController::get_all_modules() const {
    std::vector<ModuleInfo> result;
    for (const auto& [type, module] : modules_) {
        ModuleInfo info;
        info.type = type;
        info.name = module->get_name();
        info.status = module->get_status();
        result.push_back(info);
    }
    return result;
}

void UnifiedController::set_error_callback(std::function<void(const std::string&)> callback) {
    error_callback_ = callback;
}

void UnifiedController::set_progress_callback(std::function<void(int, const std::string&)> callback) {
    progress_callback_ = callback;
}

std::string UnifiedController::get_last_error() const {
    return last_error_;
}

std::string UnifiedController::generate_session_id() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
    
    // Add random suffix
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 9999);
    ss << "_" << dis(gen);
    
    return ss.str();
}

void UnifiedController::report_error(const std::string& error) {
    last_error_ = error;
    if (error_callback_) {
        error_callback_(error);
    }
}

void UnifiedController::report_progress(int progress, const std::string& message) {
    if (progress_callback_) {
        progress_callback_(progress, message);
    }
}

} // namespace narrativeforge
