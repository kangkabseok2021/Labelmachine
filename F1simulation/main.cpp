#include "laptimesim.h"


// Main program
int main() {
    std::cout << "=== Audi F1 Lap Time Simulator ===\n";
    std::cout << "Simplified Physics Model for Vehicle Development\n\n";
    
    LapTimeSimulator simulator;
    
    simulator.printVehicleSetup();
    
    // Create a simplified track (inspired by Monaco-style circuit)
    simulator.addTrackSegment(200, 0, 0, "straight");      // Start straight
    simulator.addTrackSegment(80, 50, 0, "right");         // Tight right
    simulator.addTrackSegment(150, 0, -2, "straight");     // Downhill straight
    simulator.addTrackSegment(100, 80, 0, "left");         // Medium left
    simulator.addTrackSegment(300, 0, 0, "straight");      // Long straight
    simulator.addTrackSegment(60, 40, 0, "right");         // Hairpin
    simulator.addTrackSegment(120, 0, 3, "straight");      // Uphill
    simulator.addTrackSegment(90, 120, 0, "left");         // Fast left
    
    // Run simulation
    simulator.runSimulation(0.01);
    
    // Analyze results
    simulator.analyzeTelemetry();
    
    std::cout << "Final Lap Time: " << std::fixed << std::setprecision(3) 
              << simulator.getLapTime() << " seconds\n";
    
    return 0;
}
