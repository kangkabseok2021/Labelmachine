#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <iomanip>

// Physical constants
const double GRAVITY = 9.81;  // m/s²
const double AIR_DENSITY = 1.225;  // kg/m³

// Vehicle state structure
struct VehicleState {
    double position;      // m
    double velocity;      // m/s
    double acceleration;  // m/s²
    double time;          // s
    double throttle;      // 0-1
    double brake;         // 0-1
};

// Vehicle parameters
struct VehicleParams {
    double mass;              // kg
    double dragCoeff;         // Cd
    double frontalArea;       // m²
    double downforceCoeff;    // Cl
    double maxPower;          // W
    double maxBrakeTorque;    // Nm
    double tireGripCoeff;     // μ
    double wheelRadius;       // m
};

// Track segment structure
struct TrackSegment {
    double length;            // m
    double radius;            // m (0 = straight)
    double inclination;       // degrees
    std::string type;         // "straight", "left", "right"
};

// F1 Lap Time Simulation Class
class LapTimeSimulator {
private:
    VehicleParams vehicle;
    std::vector<TrackSegment> track;
    std::vector<VehicleState> telemetry;
    double totalTime;
    
public:
    LapTimeSimulator() {
        // Initialize default F1-like vehicle parameters
        vehicle.mass = 798.0;              // kg (min F1 weight)
        vehicle.dragCoeff = 0.7;
        vehicle.frontalArea = 1.5;         // m²
        vehicle.downforceCoeff = 3.5;
        vehicle.maxPower = 750000;         // 750 kW (1000 HP)
        vehicle.maxBrakeTorque = 5000;     // Nm
        vehicle.tireGripCoeff = 1.8;
        vehicle.wheelRadius = 0.33;        // m
        
        totalTime = 0.0;
    }
    
    // Add track segment
    void addTrackSegment(double length, double radius, double inclination, std::string type) {
        TrackSegment seg;
        seg.length = length;
        seg.radius = radius;
        seg.inclination = inclination;
        seg.type = type;
        track.push_back(seg);
    }
    
    // Calculate aerodynamic drag force
    double calculateDrag(double velocity) {
        return 0.5 * AIR_DENSITY * vehicle.frontalArea * vehicle.dragCoeff * velocity * velocity;
    }
    
    // Calculate downforce
    double calculateDownforce(double velocity) {
        return 0.5 * AIR_DENSITY * vehicle.frontalArea * vehicle.downforceCoeff * velocity * velocity;
    }
    
    // Calculate maximum cornering speed
    double calculateMaxCornerSpeed(double radius) {
        if (radius == 0) return 1000.0;  // Straight (no limit from cornering)
        
        // Simplified: v_max = sqrt(μ * g * r)
        // In reality, includes downforce effects
        double downforceAtSpeed = 1000.0;  // Simplified assumption
        double normalForce = vehicle.mass * GRAVITY + downforceAtSpeed;
        double maxSpeed = sqrt(vehicle.tireGripCoeff * GRAVITY * radius);
        
        return maxSpeed;
    }
    
    // Calculate traction force
    double calculateTractionForce(double velocity, double throttle) {
        if (velocity < 0.1) velocity = 0.1;  // Avoid division by zero
        double engineForce = (vehicle.maxPower / velocity) * throttle;
        double maxTractionForce = vehicle.tireGripCoeff * vehicle.mass * GRAVITY;
        
        return std::min(engineForce, maxTractionForce);
    }
    
    // Calculate brake force
    double calculateBrakeForce(double brake) {
        return vehicle.maxBrakeTorque * brake / vehicle.wheelRadius;
    }
    
    // Simulate one time step using numerical integration
    VehicleState simulateStep(VehicleState current, TrackSegment segment, double dt) {
        VehicleState next;
        
        // Determine optimal throttle/brake based on segment
        double targetSpeed = calculateMaxCornerSpeed(segment.radius);
        
        if (current.velocity < targetSpeed * 0.95) {
            current.throttle = 1.0;
            current.brake = 0.0;
        } else if (current.velocity > targetSpeed * 1.05) {
            current.throttle = 0.0;
            current.brake = 0.8;
        } else {
            current.throttle = 0.3;
            current.brake = 0.0;
        }
        
        // Calculate forces
        double dragForce = calculateDrag(current.velocity);
        double tractionForce = calculateTractionForce(current.velocity, current.throttle);
        double brakeForce = calculateBrakeForce(current.brake);
        double gravitationalForce = vehicle.mass * GRAVITY * sin(segment.inclination * M_PI / 180.0);
        
        // Net force
        double netForce = tractionForce - dragForce - brakeForce - gravitationalForce;
        
        // Acceleration = F/m
        next.acceleration = netForce / vehicle.mass;
        
        // Numerical integration (Euler method)
        next.velocity = current.velocity + next.acceleration * dt;
        next.position = current.position + current.velocity * dt;
        next.time = current.time + dt;
        next.throttle = current.throttle;
        next.brake = current.brake;
        
        // Velocity constraints
        if (next.velocity < 0) next.velocity = 0;
        if (next.velocity > 100) next.velocity = 100;  // ~360 km/h max
        
        return next;
    }
    
    // Run full lap simulation
    void runSimulation(double timeStep = 0.01) {
        telemetry.clear();
        totalTime = 0.0;
        
        VehicleState state;
        state.position = 0.0;
        state.velocity = 0.0;
        state.acceleration = 0.0;
        state.time = 0.0;
        state.throttle = 0.0;
        state.brake = 0.0;
        
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
    void analyzeTelemetry() {
        if (telemetry.empty()) {
            std::cout << "No telemetry data available.\n";
            return;
        }
        
        double maxSpeed = 0.0;
        double maxAccel = 0.0;
        double maxBraking = 0.0;
        
        for (const auto& point : telemetry) {
            if (point.velocity > maxSpeed) maxSpeed = point.velocity;
            if (point.acceleration > maxAccel) maxAccel = point.acceleration;
            if (point.acceleration < maxBraking) maxBraking = point.acceleration;
        }
        
        std::cout << "=== Telemetry Analysis ===\n";
        std::cout << "Max Speed: " << std::fixed << std::setprecision(1) 
                  << maxSpeed * 3.6 << " km/h\n";
        std::cout << "Max Acceleration: " << std::setprecision(2) 
                  << maxAccel / GRAVITY << " G\n";
        std::cout << "Max Braking: " << std::setprecision(2) 
                  << std::abs(maxBraking) / GRAVITY << " G\n";
        std::cout << "========================\n\n";
    }
    
    // Get lap time
    double getLapTime() const {
        return totalTime;
    }
    
    // Print vehicle setup
    void printVehicleSetup() {
        std::cout << "=== Vehicle Setup ===\n";
        std::cout << "Mass: " << vehicle.mass << " kg\n";
        std::cout << "Max Power: " << vehicle.maxPower / 1000 << " kW\n";
        std::cout << "Drag Coefficient: " << vehicle.dragCoeff << "\n";
        std::cout << "Downforce Coefficient: " << vehicle.downforceCoeff << "\n";
        std::cout << "Tire Grip: " << vehicle.tireGripCoeff << "\n";
        std::cout << "====================\n\n";
    }
};

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
