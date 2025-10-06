/**
 * @file labelmachine.cpp
 * @brief ESPERA LM-3000 Labeling Machine Control System
 * @version 2.1.0
 * @date 2025-10-05
 *
 * @copyright Copyright (c) 2025 ESPERA Industrial Solutions GmbH
 *
 * This software controls the LM-3000 series industrial labeling machine.
 * It manages machine states, sensor monitoring, and production operations.
 *
 * Hardware Interface:
 * - Conveyor belt motor controller (PWM speed control)
 * - Label applicator pneumatic system
 * - Product detection sensors (photoelectric)
 * - Temperature monitoring system
 *
 * Safety Features:
 * - Emergency stop capability
 * - Temperature monitoring
 * - Label supply tracking
 * - Error state management
 */
#ifndef LABELINGMACHINE_H
#define LABELINGMACHINE_H

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <iomanip>

// Configuration Constants
namespace Config {
    constexpr int DEFAULT_SPEED = 150;          // mm/s - Normal operating speed
    constexpr int MAX_SPEED = 300;              // mm/s - Maximum safe speed
    constexpr int MIN_SPEED = 50;               // mm/s - Minimum operating speed
    constexpr int MAINTENANCE_SPEED = 20;       // mm/s - Speed for maintenance mode
    constexpr int INITIAL_LABEL_COUNT = 1000;   // Initial labels in roll
    constexpr int LOW_LABEL_THRESHOLD = 50;      // Low label warning threshold
    constexpr double NOMINAL_TEMP = 22.5;       // °C - Normal operating temperature
    constexpr double MAX_TEMP = 65.0;           // °C - Maximum safe temperature
}

/**
 * @enum MachineState
 * @brief Represents the operational state of the labeling machine
 *
 * State transitions are controlled and validated by the LabelingMachine class.
 * Invalid state transitions are rejected to ensure safe operation.
 */
enum class MachineState {
    IDLE,           ///< Machine is powered on but not operating
    RUNNING,        ///< Machine is actively labeling products
    LOW_LABEL,      ///< Machine is actively labeling products with low label warning
    PAUSED,         ///< Machine is temporarily halted (can resume)
    ERROR,          ///< Machine has encountered an error condition
    MAINTENANCE     ///< Machine is in maintenance/calibration mode
};

/**
 * @struct SensorData
 * @brief Aggregates all sensor readings from the machine
 *
 * This structure represents the physical sensor inputs that would
 * typically come from hardware interfaces in a real industrial system.
 */
struct SensorData {
    bool productDetected;       ///< Photoelectric sensor - product present
    int conveyorSpeed;          ///< Motor speed in mm/s
    int labelRollRemaining;     ///< Remaining labels in current roll
    double temperature;         ///< System temperature in Celsius
};

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
class LabelingMachine {
private:
    // Machine State
    MachineState state;                 ///< Current operational state
    MachineState previousState;         ///< State before pause/error (for resume)

    // Sensor Interface
    SensorData sensors;                 ///< Current sensor readings
    SensorData previousSensors;         ///< Previous sensor readings

    // Production Metrics
    int productsLabeled;                ///< Total products labeled in current session
    int errorCount;                     ///< Total errors encountered

    // System Information
    std::string machineId;              ///< Unique machine identifier
    std::string firmwareVersion;        ///< Current firmware version

    /**
     * @brief Validates if requested speed is within safe operating limits
     * @param speed Requested speed in mm/s
     * @return true if speed is safe, false otherwise
     */
    bool isSpeedValid(int speed) const {
        return speed >= Config::MIN_SPEED && speed <= Config::MAX_SPEED;
    }

    /**
     * @brief Checks if machine temperature is within safe operating range
     * @return true if temperature is safe, false otherwise
     */
    bool isTemperatureSafe() const {
        return sensors.temperature < Config::MAX_TEMP;
    }

    /**
     * @brief Checks if machine temperature is within safe operating range
     * @return true if temperature is safe, false otherwise
     */
    bool isLowerLabels() const {
        return sensors.labelRollRemaining < Config::LOW_LABEL_THRESHOLD;
    }

public:
    /**
     * @brief Constructs a new LabelingMachine instance with default settings
     *
     * Initializes the machine in IDLE state with default sensor values.
     * In a real system, this would also initialize hardware interfaces.
     */
    LabelingMachine();
    /**
     * @brief Destructor - ensures machine is safely stopped
     */
    ~LabelingMachine();

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
    bool start();

    /**
     * @brief Stops the labeling machine operation
     *
     * Transitions machine to IDLE state and halts conveyor belt.
     * Can be called from any state (acts as emergency stop).
     */
    void stop();

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
    void applyLabel();

    /**
     * @brief Simulates product detection sensor
     *
     * @param detected true if product is present in labeling position
     *
     * In a real system, this would be called by an interrupt handler
     * or sensor polling routine. When a product is detected and the
     * machine is running, label application is automatically triggered.
     */
    void detectProduct(bool detected);

    /**
     * @brief Displays comprehensive machine status
     *
     * Outputs current state, sensor readings, and production metrics.
     * Useful for debugging, monitoring, and operator interface.
     */
    void printStatus() const;

    /**
     * @brief Adjusts conveyor belt speed
     *
     * @param speed Desired speed in mm/s
     * @return true if speed was changed successfully, false otherwise
     *
     * Speed can only be adjusted when machine is in RUNNING state.
     * Requested speed must be within MIN_SPEED and MAX_SPEED limits.
     */
    bool setSpeed(int speed);

    /**
     * @brief Gets current machine state
     * @return Current MachineState
     */
    MachineState getState() const;
    
    /**
     * @brief Gets current production count
     * @return Number of products labeled in current session
     */
    int getProductionCount() const;
    
    /**
     * @brief Resets production counters
     *
     * Should only be called when machine is idle.
     * Used at start of new production run.
     */
    void resetCounters();
    
    /**
     * @brief Simulates loading a new label roll
     * @param labelCount Number of labels in the new roll
     */
    void loadLabelRoll(int labelCount);
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
    bool resume();
    /**
     * @brief Pauses the labeling machine operation
     *
     * Transitions machine to PAUSE state and halts conveyor belt.
     * Can be called from RUNNING state.
     */
    bool pause();

    /**
     * @brief Enter maintenance the labeling machine
     *
     * Transitions machine to MAINTENANCE state and the conveyor speed is fixed.
     * Can be called from IDLE state.
     */
    bool enterMaintenance();
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
    bool exitMaintenance();
};


#endif // LABELINGMACHINE_H