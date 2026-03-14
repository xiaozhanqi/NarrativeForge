#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <future>
#include <nlohmann/json.hpp>

namespace narrativeforge {

using json = nlohmann::json;

struct Entity {
    std::string name;
    std::string type;
    std::map<std::string, std::string> attributes;
};

struct Relation {
    std::string source;
    std::string target;
    std::string relation_type;
    double strength;
};

struct WorldRule {
    std::string key;
    std::string value;
    std::string domain;
};

struct Foreshadowing {
    std::string id;
    int setup_chapter;
    int payoff_chapter;
    std::string type;
    std::string description;
};

struct NarrativeTensor {
    std::map<std::string, Entity> entity_space;
    std::vector<Relation> relational_topology;
    std::map<std::string, WorldRule> world_rules;
    std::map<std::string, Foreshadowing> foreshadowing_field;
    std::vector<double> style_vector;
    std::string domain;
    std::string timestamp;
    
    json to_json() const;
    static NarrativeTensor from_json(const json& j);
};

struct LearningConfig {
    std::string id;
    std::string name;
    std::string provider;
    std::string model;
    double temperature;
    double top_p;
    double frequency_penalty;
    double presence_penalty;
    int max_tokens;
    std::string chunk_strategy;
    int chunk_size;
    int overlap;
    int parallel_workers;
    std::string prompt_template;
    json output_schema;
    std::vector<std::string> input_from;
    bool domain_specific;
};

struct ValidationConfig {
    std::string id;
    std::string name;
    std::string provider;
    std::string model;
    std::string validation_type;
    std::string prompt_template;
    double threshold;
};

class CanonAnalysisEngine {
public:
    CanonAnalysisEngine(const std::string& config_path, const std::string& project_id);
    ~CanonAnalysisEngine();
    
    std::future<NarrativeTensor> run_analysis_pipeline(const std::string& text);
    
    void set_progress_callback(std::function<void(int, const std::string&)> callback);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

class NarrativeGenerationEngine {
public:
    NarrativeGenerationEngine(
        const std::string& config_path,
        const std::string& project_id,
        const NarrativeTensor& tensor
    );
    ~NarrativeGenerationEngine();
    
    std::future<std::string> run_generation_pipeline();
    
    void set_progress_callback(std::function<void(int, const std::string&)> callback);
    void set_generation_mode(const std::string& mode);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace narrativeforge
