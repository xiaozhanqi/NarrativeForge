#include "narrativeforge/core.hpp"
#include "narrativeforge/api_router.hpp"
#include "narrativeforge/api_key_manager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <future>

using namespace narrativeforge;

void print_header() {
    std::cout << "============================================================" << std::endl;
    std::cout << "NarrativeForge - Universal Narrative Engineering Platform" << std::endl;
    std::cout << "C++ Version 1.0.0" << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << std::endl;
}

void print_menu() {
    std::cout << "Main Menu:" << std::endl;
    std::cout << "1. Analyze Text (Create Narrative Tensor)" << std::endl;
    std::cout << "2. Generate Text (from Narrative Tensor)" << std::endl;
    std::cout << "3. Run Full Pipeline (Analyze + Generate)" << std::endl;
    std::cout << "4. View Sample Text" << std::endl;
    std::cout << "5. Setup API Keys" << std::endl;
    std::cout << "6. Exit" << std::endl;
    std::cout << std::endl;
}

void setup_api_keys() {
    std::cout << "\n------------------------------------------------------------" << std::endl;
    std::cout << "API Key Setup" << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << std::endl;
    
    APIKeyDatabase db;
    
    std::cout << "Enter Kimi API key: ";
    std::string kimi_key;
    std::getline(std::cin, kimi_key);
    
    if (!kimi_key.empty()) {
        std::cout << "Enter model name (default: kimi-2.5): ";
        std::string model;
        std::getline(std::cin, model);
        if (model.empty()) model = "kimi-2.5";
        
        if (db.setup_api_key("kimi", kimi_key, model)) {
            std::cout << "OK - Kimi API key saved" << std::endl;
        } else {
            std::cout << "FAILED - Failed to save Kimi API key" << std::endl;
        }
    }
    
    std::cout << "\nSetup complete!" << std::endl;
}

void analyze_text() {
    std::cout << "\n============================================================" << std::endl;
    std::cout << "TEXT ANALYSIS" << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Options:" << std::endl;
    std::cout << "1. Use sample text" << std::endl;
    std::cout << "2. Load from file" << std::endl;
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
    
    CanonAnalysisEngine analyzer("config/analyzer_schema.json", "cli_project");
    
    analyzer.set_progress_callback([](int progress, const std::string& message) {
        std::cout << "[" << std::setw(3) << progress << "%] " << message << std::endl;
    });
    
    auto future = analyzer.run_analysis_pipeline(text);
    
    try {
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
    std::cout << "\n============================================================" << std::endl;
    std::cout << "TEXT GENERATION" << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Loading narrative tensor from: narrative_tensor.json" << std::endl;
    
    std::ifstream file("narrative_tensor.json");
    if (!file.is_open()) {
        std::cout << "FAILED - Cannot open narrative_tensor.json" << std::endl;
        std::cout << "Please run analysis first." << std::endl;
        return;
    }
    
    json j;
    file >> j;
    NarrativeTensor tensor = NarrativeTensor::from_json(j);
    
    std::cout << "\nGeneration Mode:" << std::endl;
    std::cout << "1. Continuation" << std::endl;
    std::cout << "2. AU (Alternative Universe)" << std::endl;
    std::cout << "3. Cross Genre" << std::endl;
    std::cout << "4. Interactive" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Select mode (1-4): ";
    std::string choice;
    std::getline(std::cin, choice);
    
    std::string mode = "continuation";
    if (choice == "2") mode = "au";
    else if (choice == "3") mode = "cross_genre";
    else if (choice == "4") mode = "interactive";
    
    std::cout << "\nStarting generation..." << std::endl;
    
    NarrativeGenerationEngine generator("config/generator_schema.json", "cli_project", tensor);
    generator.set_generation_mode(mode);
    
    generator.set_progress_callback([](int progress, const std::string& message) {
        std::cout << "[" << std::setw(3) << progress << "%] " << message << std::endl;
    });
    
    auto future = generator.run_generation_pipeline();
    
    try {
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

int main() {
    print_header();
    
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
            std::cout << "\nPlease run 'narrativeforge_cli' and select option 5 to setup API keys." << std::endl;
            return 0;
        }
    }
    
    while (true) {
        print_menu();
        
        std::cout << "Enter your choice (1-6): ";
        std::string choice;
        std::getline(std::cin, choice);
        
        if (choice == "1") {
            analyze_text();
        } else if (choice == "2") {
            generate_text();
        } else if (choice == "3") {
            analyze_text();
            generate_text();
        } else if (choice == "4") {
            std::cout << "\n============================================================" << std::endl;
            std::cout << "SAMPLE TEXT" << std::endl;
            std::cout << "============================================================" << std::endl;
            std::cout << std::endl;
            std::cout << R"(Chapter 1: Discovery of the Star Core

Aria stood in the center of the laboratory, holding a glowing crystal in her hand. The crystal emitted a faint blue light, as if stars were flowing inside it.

"What is this?" her assistant asked.

"I don't know," Aria replied, "but its energy readings far exceed any known substance.")" << std::endl;
            std::cout << std::endl;
            std::cout << "============================================================" << std::endl;
        } else if (choice == "5") {
            setup_api_keys();
        } else if (choice == "6") {
            std::cout << "\nThank you for using NarrativeForge!" << std::endl;
            std::cout << "Goodbye!" << std::endl;
            break;
        } else {
            std::cout << "\nFAILED - Invalid choice. Please try again." << std::endl;
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.ignore();
    }
    
    return 0;
}
