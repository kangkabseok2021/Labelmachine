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
double LapTimeSimulator::calculateMaxCornerSpeed(double radius, double grip_multiplier) {
    if (radius == 0) return 1000.0;  // Straight (no limit from cornering)
    double tireGrip = vehicle.tireGripCoeff * grip_multiplier;
    // Simplified: v_max = sqrt(μ * g * r)
    // In reality, includes downforce effects
    double downforceAtSpeed = 1000.0;  // Simplified assumption
    double normalForce = vehicle.mass * GRAVITY + downforceAtSpeed;
    double maxSpeed = sqrt(tireGrip * GRAVITY * radius);
    
    return maxSpeed;
}

// Calculate traction force
double LapTimeSimulator::calculateTractionForce(double velocity, double throttle, double grip_multiplier) {
    if (velocity < 0.1) velocity = 0.1;  // Avoid division by zero
    double engineForce = (vehicle.maxPower / velocity) * throttle;
    double maxTractionForce = vehicle.tireGripCoeff * grip_multiplier * vehicle.mass * GRAVITY;
    
    return std::min(engineForce, maxTractionForce);
}

// Calculate grip_multiplier from VehicleState
double LapTimeSimulator::calculateGripMultiplier(const VehicleState& state) {
    double static_load = vehicle.mass * GRAVITY;
    // double grip_multiplier = std::sqrt(state.frontLoad/(static_load * vehicle.weightDistFront));
    // grip_multiplier = std::min(grip_multiplier, std::sqrt(state.rearLoad/(static_load * vehicle.weightDistRear)));    

    double grip_multiplier = std::sqrt((state.frontLoad + state.rearLoad)/static_load);
    //grip_multiplier = 1.0;
    if(state.tireTemp < 80.0) grip_multiplier *= 0.7 + 0.3 *(state.tireTemp/80.0);
    else if(state.tireTemp > 120.0) grip_multiplier *= 1.0 - 0.5 * ((state.tireTemp - 120.0)/80.0);
    return grip_multiplier;
}


// Calculate brake force
double LapTimeSimulator::calculateBrakeForce(double brake) {
    return vehicle.maxBrakeTorque * brake / vehicle.wheelRadius;
}

// Simulate one time step using numerical integration
double LapTimeSimulator::calculateDerivartives(VehicleState& state, const TrackSegment& segment) {
    
    // Determine optimal throttle/brake based on segment
    double grip_multiplier = calculateGripMultiplier(state);
    double targetSpeed = calculateMaxCornerSpeed(segment.radius, grip_multiplier);
    
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
    double tractionForce = calculateTractionForce(state.velocity, state.throttle, grip_multiplier);
    double brakeForce = calculateBrakeForce(state.brake);
    double gravitationalForce = vehicle.mass * GRAVITY * sin(segment.inclination * M_PI / 180.0);
    
    // Net force
    double netForce = tractionForce - dragForce - brakeForce - gravitationalForce;
    
    // Acceleration = F/m
    double derivartive = netForce / vehicle.mass;
    
    return derivartive;
}

// Calculate temperature derivation for current state
double LapTimeSimulator::calculateTempDerivartives(const VehicleState& state) {
    // Heating from sliding/energy dissipation: slip_energy = 4 * velocity * tractionForce
    // dT/dt = k * (slip_energy)/(tire_mass * specific_heat)
    double grip_multiplier = calculateGripMultiplier(state);
    double tractionForce = calculateTractionForce(state.velocity, state.throttle, grip_multiplier);
    double tempDerivation = (tractionForce * state.velocity * 4.0)/(TIRE_MASS * TIRE_SPECIFIC_HEAT); 
    // Tire cooling: dT/dt = -h * (T_tire - T_ambient)
    tempDerivation -= HEAT_TRANSFER_COEFF * (state.tireTemp - AMBIENT_TEMP)/TIRE_MASS;    
    return tempDerivation;
}

// next.frontLoad and rearLoad calculation
void LapTimeSimulator::calculateLoad(VehicleState& state) {
    double downforce = calculateDownforce(state.velocity);
    double totalLoad = vehicle.mass * GRAVITY + downforce;
    double loadTransfer = (state.acceleration * vehicle.mass * vehicle.centerGravity) / vehicle.wheelBase;
    state.frontLoad = totalLoad * vehicle.weightDistFront + loadTransfer;
    state.rearLoad = totalLoad * vehicle.weightDistRear - loadTransfer;
}

// next tate with current state with updated acclerlation and tireTemp 
void LapTimeSimulator::updateNextState(VehicleState& state, const VehicleState& prestate, double dt) {
    state.velocity = prestate.velocity + state.acceleration * dt;
    state.position = prestate.position + state.velocity * dt;
    state.time = prestate.time + dt;
    state.throttle = prestate.throttle;
    state.brake = prestate.brake;
    calculateLoad(state);

    // Velocity constraints
    if (state.velocity < 0) state.velocity = 0;
    if (state.velocity > 100) state.velocity = 100;  // ~360 km/h max
}

// Simulate one time step using numerical integration
VehicleState LapTimeSimulator::simulateStep(VehicleState current, TrackSegment segment, double dt) {
    //std::cout << current.time << ":" << current.position << ":" << current.velocity << "\n";

    VehicleState next;    

    VehicleState state;
    std::vector<double> acceleration(4), tempDeriv(4);
    acceleration[0] = calculateDerivartives(current, segment);
    tempDeriv[0] = calculateTempDerivartives(current);
    double dt_t = dt/2.0;
    for(int i = 0; i < 3;++i) {
        if(i == 2) dt_t = dt;
        state.acceleration = acceleration[i];
        state.tireTemp = current.tireTemp + tempDeriv[i] * dt_t;
        updateNextState(state, current, dt_t);
        acceleration[i+1] = calculateDerivartives(state, segment);
        tempDeriv[i+1] = calculateTempDerivartives(state);
    }
    // next.acceleration = netForce / vehicle.mass;
    next.acceleration = (acceleration[0] + acceleration[3])/6.0;
    next.acceleration += (acceleration[1] + acceleration[2])/3.0;
    // tempderivation
    next.tireTemp = current.tireTemp + (tempDeriv[0] + tempDeriv[3])/6.0;
    next.tireTemp += (tempDeriv[1] + tempDeriv[2])/3.0;
    updateNextState(next, current, dt);

    return next;
}
