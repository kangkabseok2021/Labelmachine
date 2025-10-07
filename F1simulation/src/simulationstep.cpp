#include "laptimesim.h"

// Calculate aerodynamic drag force
double LapTimeSimulator::calculateDrag(double velocity) {
    return 0.5 * AIR_DENSITY * vehicle.frontalArea * vehicle.dragCoeff * velocity * velocity;
}

// Calculate downforce
double LapTimeSimulator::calculateDownforce(double velocity) {
    return 0.5 * AIR_DENSITY * vehicle.frontalArea * vehicle.downforceCoeff * velocity * velocity;
}


// Calculate maximum cornering speed
double LapTimeSimulator::calculateMaxCornerSpeed(double radius, double tireTemp) {
    if (radius == 0) return 1000.0;  // Straight (no limit from cornering)
    
    double grip_multiplier = 1.0;
    if(tireTemp < 80.0) grip_multiplier = 0.7 + 0.3 *(tireTemp/80.0);
    else if(tireTemp > 120.0) grip_multiplier = 1.0 - 0.5 * ((tireTemp - 120.0)/80.0);
    // Simplified: v_max = sqrt(μ * g * r)
    // In reality, includes downforce effects
    double downforceAtSpeed = 1000.0;  // Simplified assumption
    double normalForce = vehicle.mass * GRAVITY + downforceAtSpeed;
    double maxSpeed = sqrt(vehicle.tireGripCoeff * grip_multiplier * GRAVITY * radius);
    
    return maxSpeed;
}

// Calculate traction force
double LapTimeSimulator::calculateTractionForce(double velocity, double throttle) {
    if (velocity < 0.1) velocity = 0.1;  // Avoid division by zero
    double engineForce = (vehicle.maxPower / velocity) * throttle;
    double maxTractionForce = vehicle.tireGripCoeff * vehicle.mass * GRAVITY;
    
    return std::min(engineForce, maxTractionForce);
}

// Calculate brake force
double LapTimeSimulator::calculateBrakeForce(double brake) {
    return vehicle.maxBrakeTorque * brake / vehicle.wheelRadius;
}

// Simulate one time step using numerical integration
double LapTimeSimulator::calculateDerivartives(VehicleState state, TrackSegment segment) {
    
    // Determine optimal throttle/brake based on segment
    double targetSpeed = calculateMaxCornerSpeed(segment.radius, state.tireTemp);
    
    if (state.velocity < targetSpeed * 0.95) {
        state.throttle = 1.0;
        state.brake = 0.0;
    } else if (state.velocity > targetSpeed * 1.05) {
        state.throttle = 0.0;
        state.brake = 0.8;
    } else {
        state.throttle = 0.3;
        state.brake = 0.0;
    }
    
    // Calculate forces
    double dragForce = calculateDrag(state.velocity);
    double tractionForce = calculateTractionForce(state.velocity, state.throttle);
    double brakeForce = calculateBrakeForce(state.brake);
    double gravitationalForce = vehicle.mass * GRAVITY * sin(segment.inclination * M_PI / 180.0);
    
    // Net force
    double netForce = tractionForce - dragForce - brakeForce - gravitationalForce;
    
    // Acceleration = F/m
    double derivartive = netForce / vehicle.mass;
    
    return derivartive;
}

// Calculate temperature derivation for current state
double LapTimeSimulator::calculateTempDerivartives(VehicleState state) {
    // Heating from sliding/energy dissipation: slip_energy = 4 * velocity * tractionForce
    // dT/dt = k * (slip_energy)/(tire_mass * specific_heat)
    double tractionForce = calculateTractionForce(state.velocity, state.throttle);
    double tempDerivation = (tractionForce * state.velocity * 4.0)/(TIRE_MASS * TIRE_SPECIFIC_HEAT); 
    // Tire cooling: dT/dt = -h * (T_tire - T_ambient)
    tempDerivation -= HEAT_TRANSFER_COEFF * (state.tireTemp - AMBIENT_TEMP)/TIRE_MASS;    
    return tempDerivation;
}

// Simulate one time step using numerical integration
VehicleState LapTimeSimulator::simulateStep(VehicleState current, TrackSegment segment, double dt) {
    VehicleState next;    

    VehicleState state;
    std::vector<double> acceleration(4), tempDeriv(4);
    acceleration[0] = calculateDerivartives(current, segment);
    tempDeriv[0] = calculateTempDerivartives(current);
    double dt_t = dt/2.0;
    for(int i = 0; i < 3;++i) {
        if(i == 2) dt_t = dt;
        state.velocity = current.velocity + acceleration[i] * dt_t;
        state.position = current.position + state.velocity * dt_t;
        state.time = current.time + dt_t;
        state.tireTemp = current.tireTemp + tempDeriv[i] * dt_t;
        state.throttle = current.throttle;
        state.brake = current.brake;
        acceleration[i+1] = calculateDerivartives(state, segment);
        tempDeriv[i+1] = calculateTempDerivartives(state);
    }
    // next.acceleration = netForce / vehicle.mass;
    next.acceleration = (acceleration[0] + acceleration[3])/6.0;
    next.acceleration += (acceleration[1] + acceleration[2])/3.0;

    // Numerical integration (Euler method)
    next.velocity = current.velocity + next.acceleration * dt;
    next.position = current.position + current.velocity * dt;
    next.time = current.time + dt;
    // tempderivation
    next.tireTemp = current.tireTemp + (tempDeriv[0] + tempDeriv[3])/6.0;
    next.tireTemp += (tempDeriv[1] + tempDeriv[2])/3.0;

    next.throttle = current.throttle;
    next.brake = current.brake;
    
    // Velocity constraints
    if (next.velocity < 0) next.velocity = 0;
    if (next.velocity > 100) next.velocity = 100;  // ~360 km/h max
    
    return next;
}
