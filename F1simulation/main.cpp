#include "laptimesim.h"

// Run Simulatore with setting and retune laptime.\n\n";
double runSimulator(bool islogging, double downforceCoeff) {
    LapTimeSimulator simulator;

    simulator.updateVehicleParams(4, downforceCoeff);
    if(islogging) {
        simulator.loggingEnable();
        simulator.printVehicleSetup();
    }
    
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
    // simulator.openTelemetry("telemetry_log.csv"); // Open telemetry log file

    simulator.runSimulation(0.01);

    simulator.closeTelemetry(); // Close telemetry log file
    // Analyze results
    // simulator.analyzeTelemetry();
    
    // std::cout << "Final Lap Time: " << std::fixed << std::setprecision(3) 
    //          << simulator.getLapTime() << " seconds\n";

    // simulator.printVehicleSetup();    
    return simulator.getLapTime();
}

int openmp_run(std::vector<double>& inputs) {
    // Run multiple simulations in parallel with different downforceCoeff
    std::cout << "\n=== Running Parallel Simulations with Varying Downforce Coeff with OpenMP\n";
    std::vector<std::vector<double>> cresults;

    #pragma omp parallel for
    for(double coeff:inputs) {
        double result = runSimulator(false, coeff);
        cresults.push_back({coeff, result});
    }
    
    std::cout << "All threads have finished and results:\n";
    for(std::vector<double> res:cresults) {
        std::cout<< "laptime at " << res[0] << ": "  
                 << std::fixed << std::setprecision(3) << res[1] << " seconds\n";
    }
    return 0;
}

int async_parallel_run(std::vector<double>& inputs) {
    // Run multiple simulations in parallel with different downforceCoeff
    std::cout << "\n=== Running Parallel Simulations with Varying Downforce Coeff with Async\n";

    std::vector<std::future<double>> resultsf;
    std::vector<std::vector<double>> cresults;
    // std::vector<std::thread> workers;
    for(double coeff:inputs) {
        resultsf.push_back(std::async(std::launch::async, runSimulator, false, coeff));
    }

    for(size_t i = 0;i < resultsf.size(); ++i) {
        double result = resultsf[i].get();
        cresults.push_back({inputs[i], result});
    }
    
    std::cout << "All threads have finished and results:\n";
    for(std::vector<double> res:cresults) {
        std::cout<< "laptime at " << res[0] << ": "  
                 << std::fixed << std::setprecision(3) << res[1] << " seconds\n";
    }
    return 0;
}

int packaged_run(std::vector<double>& inputs) {
    // Run multiple simulations in parallel with different downforceCoeff
    std::cout << "\n=== Running Parallel Simulations with Varying Downforce Coeff with Packaged_task\n";

    std::vector<std::packaged_task<double(bool, double)>> tasks;
    std::vector<std::future<double>> resultsf;
    std::vector<std::vector<double>> cresults;
    std::vector<std::thread> workers;

    for(double coeff:inputs) {
        tasks.emplace_back(runSimulator);
        resultsf.push_back(tasks.back().get_future());
        workers.emplace_back(std::move(tasks.back()), false, coeff);
    }

    for(size_t i = 0;i < resultsf.size(); ++i) {
        double result = resultsf[i].get();
        cresults.push_back({inputs[i], result});
        workers[i].join();
    }
    
    std::cout << "All threads have finished and results:\n";
    for(std::vector<double> res:cresults) {
        std::cout<< "laptime at " << res[0] << ": "  
                 << std::fixed << std::setprecision(3) << res[1] << " seconds\n";
    }
    return 0;
}

// Main program
int main() {
    std::cout << "=== Audi F1 Lap Time Simulator ===\n";
    std::cout << "Simplified Physics Model for Vehicle Development\n\n";
    double downforceCoeff = 3.5;
    double laptime = runSimulator(true, downforceCoeff);

    std::cout << "Final Lap Time at downforceCoeff at " << downforceCoeff <<": "
              << std::fixed << std::setprecision(3) << laptime << " seconds\n";
    std::vector<double> inputs;
    for(int i = 0; i<9;i++) inputs.push_back(2.5 + (double) i * 0.25);
    
    packaged_run(inputs);

    async_parallel_run(inputs);

    openmp_run(inputs);

    return 0;
}
