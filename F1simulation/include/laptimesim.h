#ifndef LAPTIMESIM_H
#define LAPTIMESIM_H

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <iomanip>

// Physical constants
const double GRAVITY = 9.81;  // m/s²
const double AIR_DENSITY = 1.225;  // kg/m³
const double TIRE_MASS = 10.0;     // kg per tire
const double TIRE_SPECIFIC_HEAT = 1000.0;  // J/(kg K)
const double HEAT_TRANSFER_COEFF = 20.0;   // W/(m² K)
const double AMBIENT_TEMP = 25.0;  // °C

// Vehicle state structure
struct VehicleState {
    double position;      // m
    double velocity;      // m/s
    double acceleration;  // m/s²
    double time;          // s
    double throttle;      // 0-1
    double brake;         // 0-1
    double tireTemp;      // °C
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
    double weightDistFront;   // %
    double weightDistRear;    // %
    double centerGravity;     // m
    double wheelBase;         // m
    double stiffnessSus;      // rates frront/rear spring
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
    LapTimeSimulator();
    
    // Add track segment
    void addTrackSegment(double length, double radius, double inclination, std::string type);
    
    // Calculate aerodynamic drag force
    double calculateDrag(double velocity);
    
    // Calculate downforce
    double calculateDownforce(double velocity);

    // Calculate maximum cornering speed
    double calculateMaxCornerSpeed(double radius, double tireTemp);

    // Calculate traction force
    double calculateTractionForce(double velocity, double throttle);
    
    // Calculate brake force
    double calculateBrakeForce(double brake);

    // Calculate derivartive from current state
    double calculateDerivartives(VehicleState state, TrackSegment segment);

    // Calculate temperature derivation for current state
    double calculateTempDerivartives(VehicleState state);
    
    // Simulate one time step using numerical integration
    VehicleState simulateStep(VehicleState current, TrackSegment segment, double dt);

    // Run full lap simulation
    void runSimulation(double timeStep);
    
    // Analyze telemetry data
    void analyzeTelemetry();
    // Get lap time

    double getLapTime() const;
    
    // Print vehicle setup
    void printVehicleSetup();
};

#endif // LAPTIMESIM_H