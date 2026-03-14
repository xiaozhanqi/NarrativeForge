#include "narrativeforge/app_controller.hpp"
#include "narrativeforge/module_interface.hpp"
#include "narrativeforge/core.hpp"
#include "narrativeforge/api_router.hpp"
#include "narrativeforge/api_key_manager.hpp"
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <algorithm>

namespace narrativeforge {

class AppController::Impl {
public:
    Impl() = default;
    
    bool initialize_modules() {
        // Initialize analyzer
        auto analyzer = std::make_shared<CanonAnalysisEngine>(
            "config/analyzer_schema.json",
            context_.project_id
        );
        if (!analyzer->initialize()) {
            return false;
        }
        modules_[ModuleType::ANALYZER] = analyzer;
        
        // Initialize generator
        auto generator = std::make_shared<NarrativeGenerationEngine>(
            "config/generator_schema.json",
            context_.project_id,
            NarrativeTensor{}
        );
        if (!generator->initialize()) {
            return false;
        }
        modules_[ModuleType::GENERATOR] = generator;
        
        return true;
    }
};

AppController& AppController::instance() {
    static AppController instance;
    return instance;
}

AppController::AppController() 
    : status_(ModuleStatus::UNINITIALIZED) {
    context_.project_id = "default";
    context_.session_id = generate_session_id();
    context_.working_dir = std::filesystem::current_path().string();
}

AppController::~AppController() {
    shutdown();
}

bool AppController::initialize() {
    if (status_ != ModuleStatus::UNINITIALIZED) {
        return false;
    }
    
    status_ = ModuleStatus::INITIALIZING;
    
    try {
        // Create data directory
        std::filesystem::create_directories(config_.data_dir);
        
        // Initialize modules
        if (!pImpl_->initialize_modules()) {
            status_ = ModuleStatus::ERROR;
            return false;
        }
        
        status_ = ModuleStatus::READY;
        return true;
    } catch (const std::exception& e) {
        last_error_ = e.what();
        status_ = ModuleStatus::ERROR;
        return false;
    }
}

bool AppController::start() {
    if (status_ != ModuleStatus::READY) {
        return false;
    }
    
    status_ = ModuleStatus::RUNNING;
    
    // Start all modules
    for (auto& [type, module] : modules_) {
        if (!module->start()) {
            report_error("Failed to start module: " + module->get_name());
        }
    }
    
    return true;
}

bool AppController::stop() {
    if (status_ != ModuleStatus::RUNNING) {
        return false;
    }
    
    // Stop all modules
    for (auto& [type, module] : modules_) {
        module->stop();
    }
    
    status_ = ModuleStatus::READY;
    return true;
}

bool AppController::pause() {
    if (status_ != ModuleStatus::RUNNING) {
        return false;
    }
    
    for (auto& [type, module] : modules_) {
        module->pause();
    }
    
    status_ = ModuleStatus::PAUSED;
    return true;
}

bool AppController::resume() {
    if (status_ != ModuleStatus::PAUSED) {
        return false;
    }
    
    for (auto& [type, module] : modules_) {
        module->resume();
    }
    
    status_ = ModuleStatus::RUNNING;
    return true;
}

bool AppController::shutdown() {
    for (auto& [type, module] : modules_) {
        module->shutdown();
    }
    
    modules_.clear();
    status_ = ModuleStatus::SHUTDOWN;
    return true;
}

ModuleStatus::Status AppController::get_status() const {
    return status_;
}

bool AppController::load_config(const std::string& config_file) {
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

bool AppController::save_config(const std::string& config_file) {
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

bool AppController::register_module(ModuleType::Type type, std::shared_ptr<IModule> module) {
    std::lock_guard<std::mutex> lock(mutex_);
    modules_[type] = module;
    return true;
}

bool AppController::unregister_module(ModuleType::Type type) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = modules_.find(type);
    if (it != modules_.end()) {
        it->second->shutdown();
        modules_.erase(it);
        return true;
    }
    return false;
}

std::shared_ptr<IModule> AppController::get_module(ModuleType::Type type) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = modules_.find(type);
    return it != modules_.end() ? it->second : nullptr;
}

bool AppController::has_module(ModuleType::Type type) const {
    return modules_.find(type) != modules_.end();
}

std::future<NarrativeTensor> AppController::analyze_text(const std::string& text, const std::string& project_id) {
    return std::async(std::launch::async, [this, text, project_id]() {
        auto analyzer = std::dynamic_pointer_cast<CanonAnalysisEngine>(
            get_module(ModuleType::ANALYZER)
        );
        
        if (!analyzer) {
            throw std::runtime_error("Analyzer module not available");
        }
        
        analyzer->set_progress_callback([this](int progress, const std::string& message) {
            report_progress(progress, message);
        });
        
        auto future = analyzer->run_analysis_pipeline(text);
        return future.get();
    });
}

std::future<std::string> AppController::generate_text(const NarrativeTensor& tensor, const std::string& mode) {
    return std::async(std::launch::async, [this, tensor, mode]() {
        auto generator = std::dynamic_pointer_cast<NarrativeGenerationEngine>(
            get_module(ModuleType::GENERATOR)
        );
        
        if (!generator) {
            throw std::runtime_error("Generator module not available");
        }
        
        generator->set_generation_mode(mode);
        generator->set_progress_callback([this](int progress, const std::string& message) {
            report_progress(progress, message);
        });
        
        auto future = generator->run_generation_pipeline();
        return future.get();
    });
}

void AppController::set_error_callback(std::function<void(const std::string&)> callback) {
    error_callback_ = callback;
}

void AppController::report_error(const std::string& error) {
    last_error_ = error;
    if (error_callback_) {
        error_callback_(error);
    }
}

std::string AppController::get_last_error() const {
    return last_error_;
}

void AppController::set_progress_callback(std::function<void(int, const std::string&)> callback) {
    progress_callback_ = callback;
}

void AppController::report_progress(int progress, const std::string& message) {
    if (progress_callback_) {
        progress_callback_(progress, message);
    }
}

std::string AppController::generate_session_id() const {
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

// ModuleManager implementation
ModuleManager& ModuleManager::instance() {
    static ModuleManager instance;
    return instance;
}

bool ModuleManager::register_module(ModuleType::Type type, std::shared_ptr<IModule> module) {
    modules_[type] = module;
    return true;
}

bool ModuleManager::unregister_module(ModuleType::Type type) {
    auto it = modules_.find(type);
    if (it != modules_.end()) {
        modules_.erase(it);
        return true;
    }
    return false;
}

std::shared_ptr<IModule> ModuleManager::get_module(ModuleType::Type type) {
    auto it = modules_.find(type);
    return it != modules_.end() ? it->second : nullptr;
}

bool ModuleManager::has_module(ModuleType::Type type) const {
    return modules_.find(type) != modules_.end();
}

std::vector<ModuleInfo> ModuleManager::get_all_modules() const {
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

bool ModuleManager::initialize_all() {
    for (auto& [type, module] : modules_) {
        if (!module->initialize()) {
            return false;
        }
    }
    return true;
}

bool ModuleManager::start_all() {
    for (auto& [type, module] : modules_) {
        if (!module->start()) {
            return false;
        }
    }
    return true;
}

bool ModuleManager::stop_all() {
    for (auto& [type, module] : modules_) {
        module->stop();
    }
    return true;
}

bool ModuleManager::shutdown_all() {
    for (auto& [type, module] : modules_) {
        module->shutdown();
    }
    modules_.clear();
    return true;
}

void ModuleManager::set_global_error_callback(std::function<void(ModuleType::Type, const std::string&)> callback) {
    error_callback_ = callback;
}

} // namespace narrativeforge
