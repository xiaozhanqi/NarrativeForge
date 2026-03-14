#include "narrativeforge/unified_controller.hpp"
#include "narrativeforge/api_key_manager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace narrativeforge;

void print_banner() {
    std::cout << R"(
 _   _                      _   _                   _____             _              
| \ | | _____  ___   _ ___| |_| |__  ___  _ __   |_   _|__ _  _ ___| |_ ___ _ __  
|  \| |/ _ \ \/ / | / __| __| '_ \/ _ \| '_ \    | |/ _ \ \/ / __| __/ _ \ '_ \ 
| |\  |  __/>  <| \__ \ |_| | | | (_) | | | |   | |  __/>  <| |_| |  __/ |_) |
|_| \_|\___/_/\_\|___/\__|_| |_|  ___/|_| |_|   |_|\___/_/\_\__|_|\___|_.__/ 
                                                                                
)" << std::endl;
    std::cout << "Universal Narrative Engineering Platform v1.0.0" << std::endl;
    std::cout << "Unified Controller Edition" << std::endl;
    std::cout << std::endl;
}

void print_menu() {
    std::cout << "Main Menu:" << std::endl;
    std::cout << "  1. Analyze Text" << std::endl;
    std::cout << "  2. Generate Text" << std::endl;
    std::cout << "  3. Run Full Pipeline" << std::endl;
    std::cout << "  4. View Sample Text" << std::endl;
    std::cout << "  5. Setup API Keys" << std::endl;
    std::cout << "  6. Module Status" << std::endl;
    std::cout << "  7. Settings" << std::endl;
    std::cout << "  8. Exit" << std::endl;
    std::cout << std::endl;
}

void setup_api_keys() {
    std::cout << "\n=== API Key Setup ===" << std::endl;
    std::cout << std::endl;
    
    auto& controller = UnifiedController::instance();
    
    std::cout << "Enter Kimi API key: ";
    std::string kimi_key;
    std::getline(std::cin, kimi_key);
    
    if (!kimi_key.empty()) {
        std::cout << "Enter model name (default: kimi-2.5): ";
        std::string model;
        std::getline(std::cin, model);
        if (model.empty()) model = "kimi-2.5";
        
        if (controller.setup_api_key("kimi", kimi_key, model)) {
            std::cout << "OK - Kimi API key saved" << std::endl;
        } else {
            std::cout << "FAILED - Could not save API key" << std::endl;
        }
    }
    
    std::cout << "\nSetup complete!" << std::endl;
}

void analyze_text() {
    std::cout << "\n=== Text Analysis ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Options:" << std::endl;
    std::cout << "  1. Use sample text" << std::endl;
    std::cout << "  2. Load from file" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Select option (1-2): ";
    std::string choice;
    std::getline(std::cin, choice);
    
    std::string text;
    
    if (choice == "1") {
        text = R"(Chapter 1: Discovery of the Star Core

Aria stood in the center of the laboratory, holding a glowing crystal in her hand. The crystal emitted a faint blue light, as if stars were flowing inside it.

"What is this?" her assistant asked.

"I don't know," Aria replied, "but its energy readings far exceed any known substance."

Chapter 2: Research Begins

Over the following weeks, Aria and her team conducted comprehensive research on this crystal. They discovered that it seemed to be a concentrated energy source, containing enough power to drive an entire city.

"Let's call it the Star Core," Aria suggested, "because its energy characteristics are similar to a stellar core.")";
        std::cout << "\nOK - Sample text loaded" << std::endl;
    } else if (choice == "2") {
        std::cout << "Enter file path: ";
        std::string file_path;
        std::getline(std::cin, file_path);
        
        std::ifstream file(file_path);
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            text = buffer.str();
            std::cout << "\nOK - Loaded text from " << file_path << std::endl;
        } else {
            std::cout << "\nFAILED - Cannot open file" << std::endl;
            return;
        }
    } else {
        std::cout << "\nFAILED - Invalid option" << std::endl;
        return;
    }
    
    std::cout << "\nText length: " << text.size() << " characters" << std::endl;
    std::cout << "Starting analysis..." << std::endl;
    
    try {
        auto& controller = UnifiedController::instance();
        
        controller.set_progress_callback([](int progress, const std::string& message) {
            std::cout << "[" << std::setw(3) << progress << "%] " << message << std::endl;
        });
        
        auto future = controller.analyze_text(text);
        NarrativeTensor tensor = future.get();
        
        std::cout << "\n[Narrative Tensor Summary]" << std::endl;
        std::cout << "------------------------------------------------------------" << std::endl;
        std::cout << "Entity Space: " << tensor.entity_space.size() << " entities" << std::endl;
        std::cout << "Relational Topology: " << tensor.relational_topology.size() << " relations" << std::endl;
        std::cout << "World Rules: " << tensor.world_rules.size() << " rules" << std::endl;
        std::cout << "Foreshadowing: " << tensor.foreshadowing_field.size() << " items" << std::endl;
        std::cout << "Style Vector: " << tensor.style_vector.size() << " dimensions" << std::endl;
        
        // Save tensor
        std::ofstream out("narrative_tensor.json");
        out << tensor.to_json().dump(2);
        std::cout << "\nOK - Narrative tensor saved to: narrative_tensor.json" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "\nFAILED - Analysis failed: " << e.what() << std::endl;
    }
}

void generate_text() {
    std::cout << "\n=== Text Generation ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Loading narrative tensor from: narrative_tensor.json" << std::endl;
    
    std::ifstream file("narrative_tensor.json");
    if (!file.is_open()) {
        std::cout << "FAILED - Cannot open narrative_tensor.json" << std::endl;
        std::cout << "Please run analysis first." << std::endl;
        return;
    }
    
    nlohmann::json j;
    file >> j;
    NarrativeTensor tensor = NarrativeTensor::from_json(j);
    
    std::cout << "\nGeneration Mode:" << std::endl;
    std::cout << "  1. Continuation" << std::endl;
    std::cout << "  2. AU (Alternative Universe)" << std::endl;
    std::cout << "  3. Cross Genre" << std::endl;
    std::cout << "  4. Interactive" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Select mode (1-4): ";
    std::string choice;
    std::getline(std::cin, choice);
    
    std::string mode = "continuation";
    if (choice == "2") mode = "au";
    else if (choice == "3") mode = "cross_genre";
    else if (choice == "4") mode = "interactive";
    
    std::cout << "\nStarting generation..." << std::endl;
    
    try {
        auto& controller = UnifiedController::instance();
        
        controller.set_progress_callback([](int progress, const std::string& message) {
            std::cout << "[" << std::setw(3) << progress << "%] " << message << std::endl;
        });
        
        auto future = controller.generate_text(tensor, mode);
        std::string generated_text = future.get();
        
        std::cout << "\n[Generated Text Preview]" << std::endl;
        std::cout << "------------------------------------------------------------" << std::endl;
        std::cout << generated_text.substr(0, 500) << "..." << std::endl;
        std::cout << "------------------------------------------------------------" << std::endl;
        std::cout << "Total length: " << generated_text.size() << " characters" << std::endl;
        
        // Save generated text
        std::ofstream out("generated_output.txt");
        out << generated_text;
        std::cout << "\nOK - Generated text saved to: generated_output.txt" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "\nFAILED - Generation failed: " << e.what() << std::endl;
    }
}

void run_full_pipeline() {
    std::cout << "\n=== Full Pipeline ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Options:" << std::endl;
    std::cout << "  1. Use sample text" << std::endl;
    std::cout << "  2. Load from file" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Select option (1-2): ";
    std::string choice;
    std::getline(std::cin, choice);
    
    std::string text;
    
    if (choice == "1") {
        text = R"(Chapter 1: Discovery of the Star Core

Aria stood in the center of the laboratory, holding a glowing crystal in her hand. The crystal emitted a faint blue light, as if stars were flowing inside it.

"What is this?" her assistant asked.

"I don't know," Aria replied, "but its energy readings far exceed any known substance."

Chapter 2: Research Begins

Over the following weeks, Aria and her team conducted comprehensive research on this crystal. They discovered that it seemed to be a concentrated energy source, containing enough power to drive an entire city.

"Let's call it the Star Core," Aria suggested, "because its energy characteristics are similar to a stellar core.")";
        std::cout << "\nOK - Sample text loaded" << std::endl;
    } else if (choice == "2") {
        std::cout << "Enter file path: ";
        std::string file_path;
        std::getline(std::cin, file_path);
        
        std::ifstream file(file_path);
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            text = buffer.str();
            std::cout << "\nOK - Loaded text from " << file_path << std::endl;
        } else {
            std::cout << "\nFAILED - Cannot open file" << std::endl;
            return;
        }
    } else {
        std::cout << "\nFAILED - Invalid option" << std::endl;
        return;
    }
    
    std::cout << "\nGeneration Mode:" << std::endl;
    std::cout << "  1. Continuation" << std::endl;
    std::cout << "  2. AU (Alternative Universe)" << std::endl;
    std::cout << "  3. Cross Genre" << std::endl;
    std::cout << "  4. Interactive" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Select mode (1-4): ";
    std::string mode_choice;
    std::getline(std::cin, mode_choice);
    
    std::string mode = "continuation";
    if (mode_choice == "2") mode = "au";
    else if (mode_choice == "3") mode = "cross_genre";
    else if (mode_choice == "4") mode = "interactive";
    
    std::cout << "\nStarting full pipeline..." << std::endl;
    
    try {
        auto& controller = UnifiedController::instance();
        
        controller.set_progress_callback([](int progress, const std::string& message) {
            std::cout << "[" << std::setw(3) << progress << "%] " << message << std::endl;
        });
        
        auto future = controller.run_full_pipeline(text, mode);
        std::string generated_text = future.get();
        
        std::cout << "\n[Generated Text Preview]" << std::endl;
        std::cout << "------------------------------------------------------------" << std::endl;
        std::cout << generated_text.substr(0, 500) << "..." << std::endl;
        std::cout << "------------------------------------------------------------" << std::endl;
        std::cout << "Total length: " << generated_text.size() << " characters" << std::endl;
        
        // Save generated text
        std::ofstream out("generated_output.txt");
        out << generated_text;
        std::cout << "\nOK - Generated text saved to: generated_output.txt" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "\nFAILED - Pipeline failed: " << e.what() << std::endl;
    }
}

void show_module_status() {
    std::cout << "\n=== Module Status ===" << std::endl;
    std::cout << std::endl;
    
    auto& controller = UnifiedController::instance();
    auto modules = controller.get_all_modules();
    
    for (const auto& info : modules) {
        std::cout << "  " << info.name << ": ";
        switch (info.status) {
            case ModuleStatus::UNINITIALIZED: std::cout << "Uninitialized"; break;
            case ModuleStatus::INITIALIZING: std::cout << "Initializing"; break;
            case ModuleStatus::READY: std::cout << "Ready"; break;
            case ModuleStatus::RUNNING: std::cout << "Running"; break;
            case ModuleStatus::PAUSED: std::cout << "Paused"; break;
            case ModuleStatus::ERROR: std::cout << "Error"; break;
            case ModuleStatus::SHUTDOWN: std::cout << "Shutdown"; break;
        }
        std::cout << std::endl;
    }
    
    std::cout << std::endl;
}

void show_settings() {
    std::cout << "\n=== Settings ===" << std::endl;
    std::cout << std::endl;
    
    auto& controller = UnifiedController::instance();
    auto config = controller.get_config();
    
    std::cout << "  Config Dir: " << config.config_dir << std::endl;
    std::cout << "  Data Dir: " << config.data_dir << std::endl;
    std::cout << "  Log Level: " << config.log_level << std::endl;
    std::cout << "  GUI Enabled: " << (config.enable_gui ? "Yes" : "No") << std::endl;
    std::cout << "  Web Enabled: " << (config.enable_web ? "Yes" : "No") << std::endl;
    std::cout << "  Web Port: " << config.web_port << std::endl;
    std::cout << "  Default Provider: " << config.default_provider << std::endl;
    std::cout << "  Default Model: " << config.default_model << std::endl;
    
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    print_banner();
    
    // Initialize unified controller
    auto& controller = UnifiedController::instance();
    
    if (!controller.initialize()) {
        std::cout << "FAILED - Could not initialize application" << std::endl;
        return 1;
    }
    
    // Check API setup
    APIKeyDatabase db;
    if (db.is_first_run()) {
        std::cout << "Welcome to NarrativeForge!" << std::endl;
        std::cout << "This appears to be your first run." << std::endl;
        std::cout << "You need to setup API keys before using NarrativeForge." << std::endl;
        std::cout << std::endl;
        
        std::cout << "Would you like to setup API keys now? (y/n): ";
        std::string answer;
        std::getline(std::cin, answer);
        
        if (answer == "y" || answer == "Y") {
            setup_api_keys();
        } else {
            std::cout << "\nPlease select option 5 to setup API keys." << std::endl;
        }
    }
    
    // Start controller
    if (!controller.start()) {
        std::cout << "FAILED - Could not start application" << std::endl;
        return 1;
    }
    
    // Main loop
    bool running = true;
    while (running) {
        print_menu();
        
        std::cout << "Enter your choice (1-8): ";
        std::string choice;
        std::getline(std::cin, choice);
        
        if (choice == "1") {
            analyze_text();
        } else if (choice == "2") {
            generate_text();
        } else if (choice == "3") {
            run_full_pipeline();
        } else if (choice == "4") {
            std::cout << "\n=== Sample Text ===" << std::endl;
            std::cout << R"(Chapter 1: Discovery of the Star Core

Aria stood in the center of the laboratory, holding a glowing crystal in her hand. The crystal emitted a faint blue light, as if stars were flowing inside it.

"What is this?" her assistant asked.

"I don't know," Aria replied, "but its energy readings far exceed any known substance.")" << std::endl;
        } else if (choice == "5") {
            setup_api_keys();
        } else if (choice == "6") {
            show_module_status();
        } else if (choice == "7") {
            show_settings();
        } else if (choice == "8") {
            std::cout << "\nThank you for using NarrativeForge!" << std::endl;
            std::cout << "Goodbye!" << std::endl;
            running = false;
        } else {
            std::cout << "\nFAILED - Invalid choice. Please try again." << std::endl;
        }
        
        if (running) {
            std::cout << "\nPress Enter to continue...";
            std::cin.ignore();
        }
    }
    
    // Shutdown
    controller.shutdown();
    
    return 0;
}
