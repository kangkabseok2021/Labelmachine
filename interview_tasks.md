# ESPERA LM-3000 Interview Tasks
## Software Engineer Position - Production Support Team

Welcome to the ESPERA LM-3000 interview tasks. These tasks simulate real-world scenarios you would encounter as part of our production support team. Each task includes background information, requirements, and acceptance criteria similar to our actual ticketing system.

---

## Task 1: Basic - Pause Functionality
**Priority:** Medium
**Estimated Time:** 30 minutes
**Ticket ID:** PROD-2847

### Background
Good morning! A customer is complaining that the machine cannot be paused during operation. Currently, they can only start or stop the machine completely, which resets the production session. They need the ability to temporarily pause for safety checks or minor adjustments without losing the current session state.

### Requirements
1. Add a `pause()` function to the `LabelingMachine` class
2. The function should transition the machine state to `PAUSED`
3. When paused, the conveyor speed should be set to 0
4. The machine should only be pausable when in `RUNNING` state
5. Add appropriate console output messages
6. The machine should remember its previous state for resuming

### Acceptance Criteria
- [ ] Machine can be paused while running
- [ ] Pausing stops the conveyor belt (speed = 0)
- [ ] Machine remembers it was running and can resume later
- [ ] Cannot pause from IDLE, ERROR, or MAINTENANCE states
- [ ] Appropriate feedback messages are displayed

### Bonus Points
- Implement a `resume()` function that returns to RUNNING state from PAUSED
- Ensure resume restores the previous conveyor speed

---

## Task 2: Intermediate - Maintenance Mode
**Priority:** High
**Estimated Time:** 1 hour
**Ticket ID:** MAINT-1923

### Background
The maintenance department needs a special maintenance mode for testing and calibration. They need to run the machine at a very low speed while performing adjustments, and they need to ensure that normal production operations are disabled to prevent accidental label application during maintenance.

### Requirements
1. Implement an `enterMaintenance()` function
2. This function should only work when the machine is in `IDLE` state
3. In maintenance mode, set the conveyor speed to a fixed 20 mm/s (use `Config::MAINTENANCE_SPEED`)
4. Add an `exitMaintenance()` function to return to `IDLE` state
5. Prevent normal operations (label application) while in maintenance mode
6. Update `printStatus()` to show maintenance mode information clearly
7. In maintenance mode, `setSpeed()` should be disabled (speed locked at 20 mm/s)

### Acceptance Criteria
- [ ] Cannot enter maintenance mode while machine is running
- [ ] Speed is locked at 20 mm/s during maintenance
- [ ] Normal operations (label application) are disabled
- [ ] Can safely exit maintenance mode back to IDLE
- [ ] Status display clearly indicates maintenance mode
- [ ] Attempting to change speed in maintenance mode shows warning

### Bonus Points
- Add a reason/description parameter to `enterMaintenance()` for logging
- Track time spent in maintenance mode

---

## Task 3: Advanced - Low Label Warning System
**Priority:** Critical
**Estimated Time:** 2 hours
**Ticket ID:** PROD-3156

### Background
We have a production issue! Currently, the machine only shows an error when labels run out completely, causing production line stops and downtime. The production manager wants an early warning system so operators can prepare replacement label rolls before running out completely.

### Requirements
1. Add a new machine state: `LOW_LABELS` (as a warning state)
2. When label count drops below 50, trigger a warning but continue operation
3. Display a warning message: "WARNING: Low label supply! Only X labels remaining"
4. The machine should only enter `ERROR` state when labels reach 0
5. Modify the `applyLabel()` function to implement this logic
6. Update `printStatus()` to highlight the warning state (e.g., with visual indicators)
7. The warning should be shown each time a label is applied when in LOW_LABELS state

### Acceptance Criteria
- [ ] Warning appears when label count drops to 50 or below
- [ ] Machine continues operating during warning
- [ ] Clear distinction between warning and error states in status display
- [ ] Production only stops when labels reach 0
- [ ] Each label application shows reminder about low labels
- [ ] Reloading labels (via `loadLabelRoll()`) clears the warning state

### Bonus Points
- Make the warning threshold configurable (add to Config namespace)
- Add different warning levels (e.g., warning at 50, critical warning at 20)

---

## Task 4: Expert - Production Logging System
**Priority:** High
**Estimated Time:** 3 hours
**Ticket ID:** QC-2741

### Background
Quality control needs detailed production logs for compliance and troubleshooting. Every labeling operation must be logged with relevant data. The data needs to be in a format that can be easily imported into Excel for analysis. Additionally, logs must be timestamped for audit trail purposes.

### Requirements
1. Create a logging system using `<fstream>` header
2. Write to a file named `production_log.txt`
3. Log each label application with:
   - Timestamp (use `<chrono>` and `<ctime>`)
   - Product number (sequential counter - use `productsLabeled`)
   - Machine temperature at time of labeling
   - Conveyor speed at time of labeling
   - Success or failure status
4. Add a `closeLog()` function to properly close the file
5. Handle file errors gracefully (check if file opened successfully)
6. Format: CSV style for easy import into Excel (comma-separated)
7. Include a header row with column names

### CSV Format Example
```csv
Timestamp,ProductID,Temperature,Speed,Status
2025-10-05 14:23:15,1,22.5,150,SUCCESS
2025-10-05 14:23:16,2,22.6,150,SUCCESS
2025-10-05 14:23:17,3,22.7,150,SUCCESS
```

### Acceptance Criteria
- [ ] All labeling operations are logged (both success and failure)
- [ ] Log file is created automatically when machine starts
- [ ] Data is formatted consistently as CSV
- [ ] File is properly closed when machine stops
- [ ] Error handling for file operations (e.g., cannot create file)
- [ ] Timestamps are human-readable
- [ ] Header row is written at the start of the log file

### Bonus Points
- Append to existing log file instead of overwriting
- Add machine ID to the log entries
- Include a `getLogFilename()` method that generates unique log names (e.g., with date)

---

## Task 5: Real-World - Multi-Language Support
**Priority:** Medium
**Estimated Time:** 2-3 hours
**Ticket ID:** I18N-1547

### Background
We have international customers! A client in the UK needs the machine interface in English. We currently have all messages hardcoded in German. We need to implement a language system that supports both German and English, and potentially more languages in the future. The system should be maintainable and easy to extend.

### Requirements
1. Create an `enum Language { GERMAN, ENGLISH }`
2. Add a `currentLanguage` member variable to the class
3. Implement a `setLanguage(Language lang)` function
4. Modify ALL output messages (`std::cout`) to support both languages
5. Use a helper function `getMessage(const std::string& messageKey)` that returns the appropriate text
6. Default language should be German
7. Consider using `std::map` or switch statement for translations
8. Do not translate status display box borders or formatting characters

### Messages to Translate
- Machine started/stopped
- Label applied
- Product detected
- Speed changed
- Status display field labels
- Error messages (no labels, invalid speed, etc.)
- Warning messages
- System messages (initialization, shutdown)

### Acceptance Criteria
- [ ] All output messages available in both languages
- [ ] Language can be switched at runtime
- [ ] No hardcoded strings in main functions (except debug/logging keys)
- [ ] Clean, maintainable code structure
- [ ] Default language is German
- [ ] `setLanguage()` provides feedback in the new language

### Bonus Points
- Store translations in a `std::map<std::string, std::map<Language, std::string>>`
- Add a third language (e.g., French, Spanish)
- Create a separate `MessageTranslator` class to handle translations

### Example Structure
```cpp
std::map<std::string, std::map<Language, std::string>> messages = {
    {"machine_started", {
        {Language::GERMAN, "Maschine gestartet"},
        {Language::ENGLISH, "Machine started"}
    }},
    {"machine_stopped", {
        {Language::GERMAN, "Maschine gestoppt"},
        {Language::ENGLISH, "Machine stopped"}
    }}
};
```

---

## Task 6: Advanced - Speed Ramping System
**Priority:** Medium
**Estimated Time:** 2 hours
**Ticket ID:** PROD-2934

### Background
The current system changes speed instantly, which can cause mechanical stress on the conveyor belt system and may damage products. We need a gradual speed ramping system that smoothly transitions from one speed to another over a specified duration.

### Requirements
1. Create a `rampSpeed(int targetSpeed, int durationMs)` function
2. The function should gradually change speed from current to target over the duration
3. Speed should change in small increments (e.g., 10 mm/s steps)
4. Use `std::this_thread::sleep_for()` to control timing
5. During ramping, machine should remain in RUNNING state
6. Display progress messages during ramping
7. Validate that target speed is within safe limits
8. Ramping should be interruptible by stop() or pause()

### Acceptance Criteria
- [ ] Speed changes gradually, not instantly
- [ ] Ramping completes in approximately the specified duration
- [ ] Target speed is validated before ramping begins
- [ ] Progress messages indicate ramping is occurring
- [ ] Machine continues operation during speed ramp
- [ ] Can be interrupted safely

### Bonus Points
- Add acceleration profiles (linear, ease-in, ease-out)
- Calculate optimal step size based on duration
- Add a `getCurrentRampStatus()` function

---

## Task 7: Expert - Error Recovery System
**Priority:** Critical
**Estimated Time:** 3-4 hours
**Ticket ID:** PROD-3201

### Background
When the machine encounters an error, it currently requires manual intervention to reset. We need an automatic error recovery system that can handle certain types of errors automatically, and provide better diagnostics for errors that require operator intervention.

### Requirements
1. Create an `enum ErrorType { LABEL_EMPTY, TEMPERATURE_HIGH, SENSOR_FAULT, MECHANICAL_FAULT }`
2. Add an `errorType` member variable to track the current error
3. Implement an `attemptRecovery()` function that tries to recover from errors
4. Add an error history log (store last 10 errors with timestamps)
5. Implement different recovery strategies:
   - `LABEL_EMPTY`: Check if labels have been reloaded, if yes, clear error
   - `TEMPERATURE_HIGH`: Wait for temperature to drop below threshold, then clear
   - `SENSOR_FAULT`: Attempt to re-read sensors
   - `MECHANICAL_FAULT`: Cannot auto-recover, require manual reset
6. Add a `getErrorHistory()` function that returns formatted error history
7. Update `printStatus()` to show current error type and recovery attempts

### Acceptance Criteria
- [ ] Different error types are tracked separately
- [ ] Automatic recovery works for recoverable errors
- [ ] Manual intervention required for critical errors
- [ ] Error history is maintained and accessible
- [ ] Recovery attempts are logged
- [ ] Status display shows error type and recovery status
- [ ] Maximum 3 automatic recovery attempts before requiring manual intervention

### Bonus Points
- Add error codes (e.g., E001, E002) for customer support reference
- Implement a `manualReset()` function with password/confirmation
- Add error statistics (count of each error type)

---

## Task 8: Intermediate - Configuration System
**Priority:** Medium
**Estimated Time:** 1.5-2 hours
**Ticket ID:** CONFIG-1829

### Background
Different customers have different requirements for speed limits, temperature thresholds, and other operational parameters. Instead of hardcoding these values in the `Config` namespace, we need a configuration system that can load settings from a file at startup.

### Requirements
1. Create a configuration file `machine_config.ini` with key-value pairs
2. Implement a `loadConfig(const std::string& filename)` function
3. Support configuration of:
   - `DEFAULT_SPEED`
   - `MAX_SPEED`
   - `MIN_SPEED`
   - `MAX_TEMP`
   - `INITIAL_LABEL_COUNT`
   - `LOW_LABEL_THRESHOLD` (if you implemented Task 3)
4. Use `<fstream>` to read the file
5. Parse simple `KEY=VALUE` format
6. Provide default values if config file is missing or invalid
7. Add a `saveConfig(const std::string& filename)` function to write current settings

### Config File Format Example
```ini
DEFAULT_SPEED=150
MAX_SPEED=300
MIN_SPEED=50
MAX_TEMP=65.0
INITIAL_LABEL_COUNT=1000
LOW_LABEL_THRESHOLD=50
```

### Acceptance Criteria
- [ ] Configuration is loaded from file at initialization
- [ ] Missing config file uses hardcoded defaults
- [ ] Invalid values in config file are rejected (with warnings)
- [ ] Configuration can be saved back to file
- [ ] Error handling for file I/O operations
- [ ] Settings are validated (e.g., MAX_SPEED > MIN_SPEED)

### Bonus Points
- Support comments in config file (lines starting with `#`)
- Add a `reloadConfig()` function to reload settings at runtime
- Validate interdependent settings (e.g., default speed must be between min and max)

---

## Task 9: Advanced - Production Statistics Dashboard
**Priority:** Low
**Estimated Time:** 2-3 hours
**Ticket ID:** ANALYTICS-921

### Background
Management wants better visibility into production efficiency. We need a statistics system that tracks and displays key performance indicators (KPIs) including throughput, uptime, error rates, and efficiency metrics.

### Requirements
1. Create a `ProductionStats` struct to hold:
   - Total runtime (seconds)
   - Total products labeled
   - Average labels per minute
   - Error rate (percentage)
   - Uptime percentage (running time / total time)
2. Track timestamps for state changes
3. Implement a `getStatistics()` function that calculates and returns current stats
4. Add a `printStatistics()` function for formatted display
5. Update statistics in real-time as machine operates
6. Calculate efficiency: (products labeled / time in running state)

### Acceptance Criteria
- [ ] Statistics are tracked accurately
- [ ] Calculations are mathematically correct
- [ ] Statistics display is well-formatted and readable
- [ ] Runtime is tracked in seconds
- [ ] Efficiency metrics are calculated correctly
- [ ] Statistics persist across pause/resume

### Bonus Points
- Add hourly production goals and progress tracking
- Calculate downtime by reason (errors, maintenance, paused)
- Export statistics to CSV file
- Add OEE (Overall Equipment Effectiveness) calculation

---

## Task 10: Expert - Batch Production System
**Priority:** High
**Estimated Time:** 3-4 hours
**Ticket ID:** PROD-3312

### Background
Customers often run specific production batches with defined quantities. We need a batch management system that allows operators to define a batch size, track progress, and automatically stop when the batch is complete.

### Requirements
1. Add batch management members:
   - `int currentBatchSize` (target)
   - `int currentBatchProgress` (labeled so far)
   - `bool batchModeActive`
   - `std::string batchId` (unique identifier)
2. Implement `startBatch(const std::string& batchId, int quantity)` function
3. Track progress automatically during label application
4. When batch is complete, stop machine automatically
5. Add `getBatchProgress()` function returning percentage complete
6. Update `printStatus()` to show batch information when active
7. Add `cancelBatch()` function to abort current batch
8. Prevent starting new batch while one is active

### Acceptance Criteria
- [ ] Batch can be started with ID and quantity
- [ ] Progress is tracked automatically
- [ ] Machine stops automatically when batch completes
- [ ] Status display shows batch progress
- [ ] Batch can be cancelled
- [ ] Cannot start overlapping batches
- [ ] Completion message includes batch summary

### Bonus Points
- Save batch history to file (batch ID, quantity, completion time, errors)
- Add estimated time to completion based on current speed
- Support multiple batch profiles with different speeds/settings

---

## General Tips for All Tasks

### Code Quality Expectations
- **Documentation**: Add clear comments and doxygen-style documentation
- **Error Handling**: Check preconditions and handle errors gracefully
- **Const Correctness**: Use `const` where appropriate
- **Naming**: Follow existing naming conventions
- **Testing**: Demonstrate your implementation in `main()`

### What We're Looking For
1. **Problem Understanding**: Do you understand the requirement?
2. **Design Decisions**: Are your architectural choices sound?
3. **Code Quality**: Is the code clean, readable, and maintainable?
4. **Error Handling**: Do you handle edge cases and errors?
5. **Testing**: Can you demonstrate and validate your solution?

### How to Approach
1. Read the entire task description carefully
2. Understand the acceptance criteria
3. Plan your approach before coding
4. Implement incrementally
5. Test thoroughly
6. Update documentation

### Compilation
```bash
g++ -std=c++11 -o labelmachine labelmachine.cpp
./labelmachine
```

### Good Luck!
Remember: In a real production environment, reliability and maintainability are more important than clever code. Write code that your teammates can understand and maintain.

---

**Notes:**
- Tasks are ordered roughly by difficulty
- You don't need to complete all tasks
- Focus on code quality over quantity
- Ask questions if requirements are unclear
- Real interview would include code review discussion
