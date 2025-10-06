#include "labelmachine.h"
/**
 * @class LabelingMachine
 * @brief Main controller class for the ESPERA LM-3000 labeling machine
 *
 * This class encapsulates all machine control logic including state management,
 * sensor monitoring, and production operations. It provides a safe API for
 * machine operation while enforcing business rules and safety constraints.
 *
 * Thread Safety: This class is NOT thread-safe. External synchronization
 * required if accessed from multiple threads.
 *
 * Example Usage:
 * @code
 *   LabelingMachine machine;
 *   machine.start();
 *   machine.detectProduct(true);
 *   machine.printStatus();
 *   machine.stop();
 * @endcode
 */
LabelingMachine::LabelingMachine()
    : state(MachineState::IDLE)
    , previousState(MachineState::IDLE)
    , sensors({false, 0, Config::INITIAL_LABEL_COUNT, Config::NOMINAL_TEMP})
    , productsLabeled(0)
    , errorCount(0)
    , machineId("LM3000-001")
    , firmwareVersion("v2.1.0")
{
    std::cout << "[SYSTEM] Machine initialized: " << machineId
              << " (Firmware: " << firmwareVersion << ")\n";
}

/**
* @brief Destructor - ensures machine is safely stopped
 */
LabelingMachine::~LabelingMachine() {
    if (state == MachineState::RUNNING) {
        stop();
    }
    std::cout << "[SYSTEM] Machine shutdown complete\n";
}

/**
    * @brief Starts the labeling machine operation
 *
 * Transitions machine from IDLE to RUNNING state and sets conveyor
 * to default operating speed. Validates preconditions before starting.
 *
 * @return true if machine started successfully, false otherwise
 *
 * Preconditions:
 * - Machine must be in IDLE state
 * - Temperature must be within safe range
 * - Label supply must be available
 */
bool LabelingMachine::start() {
    if (state != MachineState::IDLE) {
        std::cout << "[WARNING] Cannot start machine - not in IDLE state\n";
        return false;
    }

    if (!isTemperatureSafe()) {
        std::cout << "[ERROR] Cannot start - temperature too high: "
                  << sensors.temperature << "°C\n";
        state = MachineState::ERROR;
        return false;
    }

    if (sensors.labelRollRemaining == 0) {
        std::cout << "[ERROR] Cannot start - no labels available\n";
        state = MachineState::ERROR;
        return false;
    }

    state = MachineState::RUNNING;
    previousState = MachineState::RUNNING;
    if (isLowerLabels()) {
        std::cout << "[WARNING] Low label warning - Labels remaining: "
                  << sensors.labelRollRemaining << "\n";
        state = MachineState::LOW_LABEL;
    }    
    sensors.conveyorSpeed = Config::DEFAULT_SPEED;
    std::cout << "[INFO] Machine started - Speed: " << Config::DEFAULT_SPEED << " mm/s\n";
    return true;
}

/**
 * @brief Stops the labeling machine operation
 *
 * Transitions machine to IDLE state and halts conveyor belt.
 * Can be called from any state (acts as emergency stop).
 */
void LabelingMachine::stop() {
    previousState = state;
    state = MachineState::IDLE;
    sensors.conveyorSpeed = 0;
    std::cout << "[INFO] Machine stopped - Total labeled: "
              << productsLabeled << "\n";
}

/**
 * @brief Applies a label to the detected product
 *
 * This is the core production function. It should only be called when
 * a product is detected and the machine is in RUNNING state.
 *
 * The function:
 * 1. Validates machine state
 * 2. Checks label availability
 * 3. Decrements label count
 * 4. Increments production counter
 * 5. Provides feedback
 *
 * @note In a real system, this would trigger pneumatic actuators
 *       and wait for label application confirmation
 */
void LabelingMachine::applyLabel() {
    if (state != MachineState::RUNNING && state != MachineState::LOW_LABEL) {
        return;
    }
    if (!sensors.productDetected) {
        return;
    }

    if (sensors.labelRollRemaining > 0) {
        sensors.labelRollRemaining--;
        productsLabeled++;
        if (isLowerLabels()) {
            std::cout << "[WARNING] Low label warning - Labels remaining: "
                      << sensors.labelRollRemaining << "\n";
            state = MachineState::LOW_LABEL;
        } 
        // Simulate temperature increase from operation
        sensors.temperature += 0.1;

        std::cout << "[PRODUCTION] Label applied - Product #"
                  << productsLabeled
                  << " | Labels remaining: " << sensors.labelRollRemaining
                  << "\n";
    } else {
        state = MachineState::ERROR;
        errorCount++;
        sensors.conveyorSpeed = 0;
        std::cout << "[ERROR] Label application failed - Roll empty!\n";
    }
}

/**
 * @brief Simulates product detection sensor
 *
 * @param detected true if product is present in labeling position
 *
 * In a real system, this would be called by an interrupt handler
 * or sensor polling routine. When a product is detected and the
 * machine is running, label application is automatically triggered.
 */
void LabelingMachine::detectProduct(bool detected) {
    sensors.productDetected = detected;
    if (detected && (state == MachineState::RUNNING || state == MachineState::LOW_LABEL)) {
        std::cout << "[SENSOR] Product detected at labeling position\n";
        applyLabel();
    }
}

/**
 * @brief Displays comprehensive machine status
 *
 * Outputs current state, sensor readings, and production metrics.
 * Useful for debugging, monitoring, and operator interface.
 */
void LabelingMachine::printStatus() const {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════╗\n";
    std::cout << "║     ESPERA LM-3000 Machine Status            ║\n";
    std::cout << "╠══════════════════════════════════════════════╣\n";
    std::cout << "║ Machine ID: " << std::left << std::setw(30) << machineId << "   ║\n";
    std::cout << "║ State:      " << std::left << std::setw(30);

    switch(state) {
        case MachineState::IDLE:        std::cout << "IDLE"; break;
        case MachineState::RUNNING:     std::cout << "RUNNING"; break;
        case MachineState::LOW_LABEL:   std::cout << "LOW_LABEL"; 
            std::cout << "   ║\n";
            std::cout << "║ [WARNING] Low label warning               ";
            break;
        case MachineState::PAUSED:      std::cout << "PAUSED"; break;
        case MachineState::ERROR:       std::cout << "ERROR"; break;
        case MachineState::MAINTENANCE: std::cout << "MAINTENANCE"; break;
    }
    std::cout << "   ║\n";
    std::cout << "╠══════════════════════════════════════════════╣\n";
    std::cout << "║ Conveyor Speed:    " << std::setw(15) << sensors.conveyorSpeed << " mm/s      ║\n";
    std::cout << "║ Labels Remaining:  " << std::setw(15) << sensors.labelRollRemaining << "           ║\n";
    std::cout << "║ Products Labeled:  " << std::setw(15) << productsLabeled << "           ║\n";
    std::cout << "║ Temperature:       " << std::setw(15) << std::fixed << std::setprecision(1)
              << sensors.temperature << " °C        ║\n";
    std::cout << "║ Error Count:       " << std::setw(15) << errorCount << "           ║\n";
    std::cout << "╚══════════════════════════════════════════════╝\n";
    std::cout << "\n";
}

/**
 * @brief Adjusts conveyor belt speed
 *
 * @param speed Desired speed in mm/s
 * @return true if speed was changed successfully, false otherwise
 *
 * Speed can only be adjusted when machine is in RUNNING state.
 * Requested speed must be within MIN_SPEED and MAX_SPEED limits.
 */
bool LabelingMachine::setSpeed(int speed) {
    if (state != MachineState::RUNNING && state != MachineState::LOW_LABEL) {
        std::cout << "[WARNING] Cannot adjust speed - machine not running\n";
        return false;
    }

    if (!isSpeedValid(speed)) {
        std::cout << "[ERROR] Invalid speed: " << speed
                  << " mm/s (valid range: " << Config::MIN_SPEED
                  << "-" << Config::MAX_SPEED << ")\n";
        return false;
    }

    sensors.conveyorSpeed = speed;
    std::cout << "[INFO] Speed changed to " << speed << " mm/s\n";
    return true;
}

/**
 * @brief Gets current machine state
 * @return Current MachineState
 */
MachineState LabelingMachine::getState() const {
    return state;
}

/**
 * @brief Gets current production count
 * @return Number of products labeled in current session
 */
int LabelingMachine::getProductionCount() const {
    return productsLabeled;
}

/**
 * @brief Resets production counters
 *
 * Should only be called when machine is idle.
 * Used at start of new production run.
 */
void LabelingMachine::resetCounters() {
    if (state == MachineState::IDLE) {
        productsLabeled = 0;
        errorCount = 0;
        std::cout << "[INFO] Production counters reset\n";
    } else {
        std::cout << "[WARNING] Cannot reset counters while machine is operating\n";
    }
}

/**
 * @brief Simulates loading a new label roll
 * @param labelCount Number of labels in the new roll
 */
void LabelingMachine::loadLabelRoll(int labelCount) {
    if (labelCount < 0) {
        std::cout << "[ERROR] Invalid label count\n";
        return;
    }

    sensors.labelRollRemaining = labelCount;
    std::cout << "[INFO] Label roll loaded: " << labelCount << " labels\n";
    // Clear error state if it was due to empty labels
    if (state == MachineState::LOW_LABEL && labelCount >= Config::LOW_LABEL_THRESHOLD) {
        state = MachineState::RUNNING;
        std::cout << "[INFO] Low Label Warning cleared - machine is running\n";
    }

    // Clear error state if it was due to empty labels
    if (state == MachineState::ERROR && labelCount > 0) {
        state = MachineState::IDLE;
        std::cout << "[INFO] Error cleared - machine ready\n";
    }
}
