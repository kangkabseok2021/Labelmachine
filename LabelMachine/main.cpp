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

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <iomanip>
#include "labelmachine.h"

/**
 * @brief Main program demonstrating machine operation
 *
 * This simulates a typical production cycle:
 * 1. Machine initialization
 * 2. Status check
 * 3. Production run
 * 4. Speed adjustment
 * 5. Shutdown
 *
 * @return 0 on successful completion
 */
int main() {
    std::cout << "╔══════════════════════════════════════════════╗\n";
    std::cout << "║   ESPERA LM-3000 Control System v2.1.0       ║\n";
    std::cout << "║   Industrial Labeling Machine Controller     ║\n";
    std::cout << "╚══════════════════════════════════════════════╝\n\n";

    // Initialize machine
    LabelingMachine machine;

    // Display initial status
    machine.printStatus();

    // Start production
    std::cout << ">>> Starting production run...\n\n";
    if (!machine.start()) {
        std::cerr << "Failed to start machine\n";
        return 1;
    }

    // Simulate production cycle - label 5 products
    std::cout << ">>> Simulating production cycle (5 products)...\n\n";
    for (int i = 0; i < 5; i++) {
        // Simulate conveyor timing
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // Product enters labeling zone
        machine.detectProduct(true);

        // Small delay for product to exit
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        machine.detectProduct(false);
    }
    // Simulate production cycle - label 6 products with paused 4 periods
    std::cout << ">>> Simulating production cycle (6 products) with paused 4 periods...\n\n";
    for (int i = 0; i < 10; i++) {
        // Simulate conveyor timing
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        if(i == 3) machine.pause();
        if(i == 7) {
            machine.resume();
            std::cout << "\n>>> Mid-production status check on resume:\n";
            machine.printStatus();
        }
        if(i == 5) {
            // Check status PAUSED
            std::cout << "\n>>> Mid-production status (PAUSED) check:\n";
            machine.printStatus();
        }
        // Product enters labeling zone
        machine.detectProduct(true);

        // Small delay for product to exit
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        machine.detectProduct(false);
    }
    // Check status mid-production
    std::cout << "\n>>> Mid-production status check:\n";
    machine.printStatus();

    // Demonstrate speed adjustment
    std::cout << ">>> Increasing production speed...\n\n";
    machine.setSpeed(200);

    // Stop machine
    std::cout << "\n>>> Stopping machine...\n\n";
    machine.stop();

    // Final status report
    std::cout << ">>> Final status:\n";
    machine.printStatus();

    std::cout << ">>> Production session complete\n";

    return 0;
}
