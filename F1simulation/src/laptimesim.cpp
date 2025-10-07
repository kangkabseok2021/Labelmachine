#include "laptimesim.h"

// F1 Lap Time Simulation Class
LapTimeSimulator::LapTimeSimulator() {
    // Initialize default F1-like vehicle parameters
    vehicle.mass = 798.0;              // kg (min F1 weight)
    vehicle.dragCoeff = 0.7;
    vehicle.frontalArea = 1.5;         // m²
    vehicle.downforceCoeff = 3.5;
    vehicle.maxPower = 750000;         // 750 kW (1000 HP)
    vehicle.maxBrakeTorque = 5000;     // Nm
    vehicle.tireGripCoeff = 1.8;
    vehicle.wheelRadius = 0.33;        // m
    vehicle.weightDistFront = 45.0;    // %
    vehicle.weightDistRear = 55.0;     // %
    vehicle.centerGravity = 0.3;       // m
    vehicle.wheelBase = 3.6;           // m
    vehicle.stiffnessSus = 1.0;        // rates frront/rear spring
    
    totalTime = 0.0;
}

// Add track segment
void LapTimeSimulator::addTrackSegment(double length, double radius, double inclination, std::string type) {
    TrackSegment seg;
    seg.length = length;
    seg.radius = radius;
    seg.inclination = inclination;
    seg.type = type;
    track.push_back(seg);
}

// Run full lap simulation
void LapTimeSimulator::runSimulation(double timeStep = 0.01) {
    telemetry.clear();
    totalTime = 0.0;
    
    VehicleState state;
    state.position = 0.0;
    state.velocity = 0.0;
    state.acceleration = 0.0;
    state.time = 0.0;
    state.throttle = 0.0;
    state.brake = 0.0;
    state.tireTemp = 60.0;
    
    std::cout << "\n=== Starting Lap Time Simulation ===\n\n";
    
    for (size_t i = 0; i < track.size(); i++) {
        TrackSegment segment = track[i];
        double segmentStart = state.position;
        double segmentEnd = segmentStart + segment.length;
        
        std::cout << "Segment " << (i+1) << " (" << segment.type << "): ";
        std::cout << segment.length << "m";
        if (segment.radius > 0) {
            std::cout << ", R=" << segment.radius << "m";
        }
        std::cout << "\n";
        
        // Simulate through this segment
        while (state.position < segmentEnd) {
            telemetry.push_back(state);
            state = simulateStep(state, segment, timeStep);
            
            if (state.position >= segmentEnd) {
                break;
            }
        }
        
        std::cout << "  Exit speed: " << std::fixed << std::setprecision(1) 
                    << state.velocity * 3.6 << " km/h\n";
    }
    
    totalTime = state.time;
    
    std::cout << "\n=== Lap Complete ===\n";
    std::cout << "Total Lap Time: " << std::fixed << std::setprecision(3) 
                << totalTime << " seconds\n";
    std::cout << "Telemetry points recorded: " << telemetry.size() << "\n\n";
}

// Analyze telemetry data
void LapTimeSimulator::analyzeTelemetry() {
    if (telemetry.empty()) {
        std::cout << "No telemetry data available.\n";
        return;
    }
    
    double maxSpeed = 0.0;
    double maxAccel = 0.0;
    double maxBraking = 0.0;
    double maxTireTemp = 0.0;
    
    for (const auto& point : telemetry) {
        if (point.velocity > maxSpeed) maxSpeed = point.velocity;
        if (point.acceleration > maxAccel) maxAccel = point.acceleration;
        if (point.acceleration < maxBraking) maxBraking = point.acceleration;
        if (point.tireTemp > maxTireTemp) maxTireTemp = point.tireTemp;
    }
    
    std::cout << "=== Telemetry Analysis ===\n";
    std::cout << "Max Speed: " << std::fixed << std::setprecision(1) 
                << maxSpeed * 3.6 << " km/h\n";
    std::cout << "Max Acceleration: " << std::setprecision(2) 
                << maxAccel / GRAVITY << " G\n";
    std::cout << "Max Braking: " << std::setprecision(2) 
                << std::abs(maxBraking) / GRAVITY << " G\n";
    std::cout << "Max Tire Temperature: " << std::setprecision(1)
                << maxTireTemp << " °C\n"; 
    std::cout << "========================\n\n";
}

// Get lap time
double LapTimeSimulator::getLapTime() const {
    return totalTime;
}

// Print vehicle setup
void LapTimeSimulator::printVehicleSetup() {
    std::cout << "=== Vehicle Setup ===\n";
    std::cout << "Mass: " << vehicle.mass << " kg\n";
    std::cout << "Max Power: " << vehicle.maxPower / 1000 << " kW\n";
    std::cout << "Drag Coefficient: " << vehicle.dragCoeff << "\n";
    std::cout << "Downforce Coefficient: " << vehicle.downforceCoeff << "\n";
    std::cout << "Tire Grip: " << vehicle.tireGripCoeff << "\n";
    std::cout << "====================\n\n";
}