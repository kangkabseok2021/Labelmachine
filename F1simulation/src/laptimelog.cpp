#include "laptimesim.h"

// Open telemetry log file and set telemetrryLogEnabled
void LapTimeSimulator::openTelemetry(const std::string& telemetryFileName) {
    // Open log telemetry file
    telemetryLogFile.open(telemetryFileName, std::ios::out | std::ios::trunc);

    if(!telemetryLogFile) {
        std::cerr << "[ERROR] Failed to open tlelmetry log file: " << telemetryFileName << "\n";
    }
    else {
        telemetryLogEnabled = true;
        telemetryLogFile << "Time(s),Distance(m),Speed(m/s),Speed(km/h),Acceleration(m/s2),Throttle(%),Brake(%),TireTemp(°C)\n";
        std::cout << "[INFO] Telemetry log file opened: " << telemetryFileName << "\n";
    }
}

// Export current telemetry to telemetry log file
void LapTimeSimulator::exportTelemetry(const VehicleState& state) {
    if(telemetryLogEnabled) {
        if (!telemetryLogFile.is_open()) {
            std::cerr << "[ERROR] Telemetry Log file not open. Cannot export telemetry.\n";
            return;
        }
        telemetryLogFile << std::fixed << std::setprecision(3)
                         << state.time << ","
                         << state.position << ","
                         << state.velocity << ","
                         << state.velocity * 3.6 << ","
                         << state.acceleration << ","
                         << state.throttle * 100.0 << ","
                         << state.brake * 100.0 << ","
                         << state.tireTemp << "\n";
        telemetryLogFile.flush();
}

// Close telemetry log file and set telemetrryLogEnabled as fasle
void LapTimeSimulator::closeTelemetry() {
    if (telemetryLogFile.is_open()) {
        std::cout << "\n[INFO] Closing telemetry log (" << TELEMETRY_LOG_FILE_NAME << ")\n";
        telemetryLogFile.close();
        telemetryLogEnabled = false;
    }
}