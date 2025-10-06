#include "labelmachine.h"

/**
 * @brief Resumes the labeling machine operation
 *
 */
void LabelingMachine::loadConfig(const std::string& filename) {
    // For simplicity, we will simulate loading configuration from a file.
    std::ifstream infile(filename);
    std::map<std::string, std::string> temp_settings;
    std::string line;
    if(!infile.is_open()) {
        std::cout << "[WARNING] Configuration file not found or cannot be opened: " << filename << "\n";    
        std::cout << "[INFO] Using default settings.\n";
        return;
    }

    std::cout << "[INFO] Loading configuration from " << filename << "\n";
    while(std::getline(infile, line)) {
        // Skip comments and empty lines
        if(line.empty() || line[0] == '#') continue;

        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;
        std:;string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        temp_settings[key] = value;
    }
    infile.close();
    for (const auto& pair : temp_settings) {
        std::cout << "[INFO] Loaded config: " << pair.first << " = " << pair.second << "\n";
        if(pair.first == "defaultSpeed") {
            int val = std::stoi(pair.second);
            if(val >= Config::MIN_SPEED && val <= Config::MAX_SPEED) {
                config.defaultSpeed = val;
            } else {
                std::cout << "[WARNING] Invalid defaultSpeed value in config. Using default: " 
                          << config.defaultSpeed << "\n";
            }
        } 
        else if(pair.first == "maxSpeed") {
            int val = std::stoi(pair.second);
            if(val >= Config::MIN_SPEED && val <= 500) { // Arbitrary upper limit
                config.maxSpeed = val;
            } else {
                std::cout << "[WARNING] Invalid maxSpeed value in config. Using default: " 
                          << config.maxSpeed << "\n";
            }
        } 
        else if(pair.first == "minSpeed") {
            int val = std::stoi(pair.second);
            if(val >= 10 && val <= Config::MAX_SPEED) { // Arbitrary lower limit
                config.minSpeed = val;
            } else {
                std::cout << "[WARNING] Invalid minSpeed value in config. Using default: " 
                          << config.minSpeed << "\n";
            }
        } 
        else if(pair.first == "maintenanceSpeed") {
            int val = std::stoi(pair.second);
            if(val >= 5 && val <= Config::MAX_SPEED) { // Arbitrary limits
                config.maintenanceSpeed = val;
            } else {
                std::cout << "[WARNING] Invalid maintenanceSpeed value in config. Using default: " 
                          << config.maintenanceSpeed << "\n";
            }
        } 
        else if(pair.first == "initialLabelCount") {
            int val = std::stoi(pair.second);
            if(val >= 0 && val <= 10000) { // Arbitrary upper limit
                config.initialLabelCount = val;
                sensors.labelRollRemaining = val; // Initialize sensor value
            } else {
                std::cout << "[WARNING] Invalid initialLabelCount value in config. Using default: " 
                          << config.initialLabelCount << "\n";
            }
        } 
        else if(pair.first == "lowLabelThreshold") {
            int val = std::stoi(pair.second);
            if(val >= 0 && val <= 500) { // Arbitrary upper limit
                config.lowLabelThreshold = val;
            } else {
                std::cout << "[WARNING] Invalid lowLabelThreshold value in config. Using default: " 
                          << config.lowLabelThreshold << "\n";
            }
        }   
        else if(pair.first == "nominalTemperature") {
            double val = std::stod(pair.second);
            if(val >= 0.0 && val <= 100.0) { // Arbitrary limits
                config.nominalTemperature = val;
            } else {
                std::cout << "[WARNING] Invalid nominalTemperature value in config. Using default: " 
                        << config.nominalTemperature << "\n";
            }
        } 
        else if(pair.first == "maxTemperature") {
            double val = std::stod(pair.second);
            if(val >= 20.0 && val <= 150.0) { // Arbitrary limits
                config.maxTemperature = val;
            } else {
                std::cout << "[WARNING] Invalid maxTemperature value in config. Using default: " 
                          << config.maxTemperature << "\n";
            }
        }           
    }
    infile.close(); 
}   
