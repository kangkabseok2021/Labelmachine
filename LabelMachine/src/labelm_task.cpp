#include "labelmachine.h"

/**
 * @brief Resumes the labeling machine operation
 *
 * Transitions machine from PAUSED to RUNNING state and sets conveyor
 * to previous speed. Validates preconditions before resuming.
 *
 * @return true if machine resumed successfully, false otherwise
 *
 * Preconditions:
 * - Machine must be in PAUSED state
 * - Temperature must be within safe range
 * - Label supply must be available
 * - If Label is not available, transitions machine to IDLE state
 */
bool LabelingMachine::resume() {
    if (state != MachineState::PAUSED) {
        std::cout << "[WARNING] Cannot resume machine - not in PAUSED state\n";
        return false;
    }

    if (!isTemperatureSafe()) {
        std::cout << "[ERROR] Cannot start - temperature too high: "
                    << sensors.temperature << "°C\n";
        state = MachineState::ERROR;
        return false;
    }

    if (sensors.labelRollRemaining == 0) {
        std::cout << "[WARNING] Cannot resume - no labels available. To IDLE state.\n";
        previousState = state;
        state = MachineState::IDLE;
        return false;
    }

    MachineState cstate = state;
    state = previousState;
    previousState = cstate;
    sensors = previousSensors;
    if (isLowerLabels()) {
        std::cout << "[WARNING] Low label warning - Labels remaining: "
                  << sensors.labelRollRemaining << "\n";
        state = MachineState::LOW_LABEL;
    } 
    std::cout << "[INFO] Machine resumed - Speed: " << sensors.conveyorSpeed << " mm/s\n";
    return true;
}

/**
 * @brief Pauses the labeling machine operation
 *
 * Transitions machine to PAUSE state and halts conveyor belt.
 * Can be called from RUNNING state.
 */
bool LabelingMachine::pause() {
    if (state != MachineState::RUNNING && state != MachineState::LOW_LABEL) {
        std::cout << "[WARNING] Cannot pause machine - not in RUNNING state\n";
        return false;
    }

    previousState = state;
    state = MachineState::PAUSED;
    previousSensors = sensors;
    sensors.conveyorSpeed = 0;
    std::cout << "[INFO] Machine paused - Total labeled: "
                << productsLabeled << " - Remaining labels: " 
                << sensors.labelRollRemaining << "\n";
    return true;
}

/**
 * @brief Enter maintenance the labeling machine
 *
 * Transitions machine to MAINTENANCE state and the conveyor speed is fixed.
 * Can be called from IDLE state.
 */
bool LabelingMachine::enterMaintenance() {
    if (state != MachineState::IDLE) {
        std::cout << "[WARNING] Cannot enter maintenance machine - not in IDLE state\n";
        return false;
    }

    previousState = MachineState::IDLE;
    state = MachineState::MAINTENANCE;
    sensors.conveyorSpeed = Config::MAINTENANCE_SPEED;
    std::cout << "[INFO] Machine in maintenance." << "\n";
    return true;
}

/**
 * @brief Exit maintenance mode of the labeling machine
 *
 * Transitions machine from MAINTENANCE to IDLE state and sets conveyor
 * speed to 0. 
 *
 * @return true if machine exit maintenance mode successfully, false otherwise
 *
 * Preconditions:
 * - Machine must be in MAINTENANCE state
 */
bool LabelingMachine::exitMaintenance() {
    if (state != MachineState::MAINTENANCE) {
        std::cout << "[WARNING] Cannot exit maintenance mode - not in MAIINTENANCE state\n";
        return false;
    }
    previousState = state;
    state = MachineState::IDLE;
    sensors.conveyorSpeed = 0;
    std::cout << "[INFO] Machine exited from maintenance mode. Ready for operation.\n";
    return true;
}

std::string LabelingMachine::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* parts = std::localtime(&now_c);
    if(!parts) {
        return "0000-00-00 00:00:00"; // Fallback in case of error
    }

    std::stringstream ss;
    ss << std::put_time(parts, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void LabelingMachine::openLog() {
    // Open log file
    logFile.open(LOG_FILE_NAME, std::ios::out | std::ios::trunc);

    if (!logFile) {
        std::cerr << "[ERROR] Failed to open log file: " << LOG_FILE_NAME << "\n";
    }

    // Write the CSV header row
    logFile << "Timestamp,ProductID,Temperature,Speed,Status\n";
    logFile.flush();
    std::cout << "[INFO] Log file initialized and header written on: " << LOG_FILE_NAME << "\n";
}

void LabelingMachine::logEntry(const std::string& status) {
    if (!logFile.is_open()) {
        std::cerr << "[ERROR] Log file not open. Cannot log entry.\n";
        return;
    }
    int productId = productsLabeled; // Using productsLabeled as ProductID
    if(status == "FAILURE") {
        productId = productsLabeled + 1; // Next product ID for failure
    }
    std::string timestamp = getCurrentTime();
    logFile << timestamp << ","
            << productId << ","
            << std::fixed << std::setprecision(1) << sensors.temperature << ","
            << sensors.conveyorSpeed << ","
            << status << "\n";
    logFile.flush();
}

void LabelingMachine::closeLog() {
    if (logFile.is_open()) {
        std::cout << "\n[INFO] Closing product log (" << LOG_FILE_NAME << ")\n";
        std::cout << " Total products logged: " << productsLabeled << "\n";
        logFile.close();
    }
}