# ESPERA LM-3000 Interview Practice

This repository contains interview practice materials for a Software Engineer position at ESPERA Industrial Solutions GmbH.

## Overview

The codebase simulates an industrial labeling machine control system. You'll be working on realistic production scenarios that would be encountered in a real company environment.

## Files

- **`labelmachine.cpp`** - Main production-ready codebase with professional documentation
- **`interview_tasks.md`** - 10 interview tasks ranging from basic to expert level
- **`labelmachine.c`** - Original simple version (deprecated, use .cpp instead)

## Getting Started

### Compilation

```bash
g++ -std=c++11 -Wall -Wextra -o labelmachine labelmachine.cpp
```

### Running

```bash
./labelmachine
```

## What Changed from Original Code

The updated `labelmachine.cpp` includes:

### Professional Code Structure
- **Doxygen documentation** for all functions and classes
- **Namespace for constants** (Config namespace)
- **Better error handling** with validation
- **Const correctness** throughout
- **Professional formatting** with clear visual output
- **RAII principles** (destructor cleanup)

### Added Features
- Machine ID and firmware version tracking
- Temperature safety validation
- Speed limit validation with configurable ranges
- Previous state tracking (for pause/resume implementation)
- Enhanced status display with box drawing characters
- Counter reset functionality
- Label roll loading simulation
- Better separation of concerns

### Code Quality Improvements
- Comprehensive inline documentation
- Meaningful variable names
- Clear function responsibilities
- Safety checks and preconditions
- Informative user feedback
- Professional logging-style output (`[INFO]`, `[ERROR]`, `[WARNING]`)

## Interview Tasks

There are **10 tasks** in `interview_tasks.md`, organized by difficulty:

1. **Task 1 (Basic)**: Pause Functionality - 30 min
2. **Task 2 (Intermediate)**: Maintenance Mode - 1 hour
3. **Task 3 (Advanced)**: Low Label Warning System - 2 hours
4. **Task 4 (Expert)**: Production Logging System - 3 hours
5. **Task 5 (Real-World)**: Multi-Language Support - 2-3 hours
6. **Task 6 (Advanced)**: Speed Ramping System - 2 hours
7. **Task 7 (Expert)**: Error Recovery System - 3-4 hours
8. **Task 8 (Intermediate)**: Configuration System - 1.5-2 hours
9. **Task 9 (Advanced)**: Production Statistics Dashboard - 2-3 hours
10. **Task 10 (Expert)**: Batch Production System - 3-4 hours

### Recommended Approach

Start with **Tasks 1-5** as these were specifically requested. Then move on to the additional tasks (6-10) based on your available time and interest.

## Key Differences from Company Environment

### What's Simplified
- No actual hardware interface (all simulated)
- Single-threaded (real systems would use threading/async)
- No database/network communication
- Simplified error handling (real systems more robust)
- Console output instead of GUI/HMI

### What's Realistic
- Code documentation style
- Error handling patterns
- State machine design
- Safety validations
- Configuration management approach
- Logging requirements
- Multi-language support needs

## Tips for Interview Success

1. **Read the entire task** before starting to code
2. **Understand acceptance criteria** - these define "done"
3. **Test your implementation** - add demo code to `main()`
4. **Handle edge cases** - what if inputs are invalid?
5. **Document your code** - explain complex logic
6. **Ask questions** - in real interviews, clarification is encouraged

## Code Quality Checklist

Before considering a task complete, verify:

- [ ] Code compiles without warnings
- [ ] Function documentation is complete
- [ ] Error cases are handled
- [ ] Preconditions are validated
- [ ] User feedback is clear and helpful
- [ ] Code follows existing style
- [ ] Implementation tested in `main()`
- [ ] All acceptance criteria are met

## Company Coding Standards

The codebase follows these standards (common in industrial software):

### Documentation
- Doxygen-style comments for all public functions
- Brief descriptions for complex logic
- Parameter and return value documentation

### Naming Conventions
- `PascalCase` for classes and enums
- `camelCase` for functions and variables
- `UPPER_CASE` for constants
- Descriptive names (avoid abbreviations)

### Error Handling
- Validate inputs before processing
- Return `bool` for success/failure where appropriate
- Provide helpful error messages
- Fail safely (prefer IDLE/safe state on error)

### Safety
- Check preconditions (state, values, resources)
- Prevent invalid state transitions
- Validate user inputs
- Log important events

## Practice Schedule Suggestion

### Week 1: Basics
- Day 1-2: Task 1 (Pause)
- Day 3-4: Task 2 (Maintenance)
- Day 5-6: Task 3 (Low Label Warning)

### Week 2: Advanced
- Day 1-2: Task 4 (Logging)
- Day 3-5: Task 5 (Multi-language)

### Week 3: Expert (if time allows)
- Choose 2-3 tasks from Tasks 6-10 based on interest

## Common Interview Questions

Be prepared to discuss:

1. **Why did you choose this approach?** - Design decisions
2. **How would you test this?** - Testing strategy
3. **What could go wrong?** - Edge cases and error scenarios
4. **How would you extend this?** - Scalability and maintainability
5. **What are the trade-offs?** - Performance vs clarity, etc.

## Resources

### C++ References
- [cppreference.com](https://en.cppreference.com/) - C++ standard library
- State machine patterns
- File I/O with fstream
- Time handling with chrono

### Industrial Programming Concepts
- SCADA systems
- PLC programming concepts (state machines)
- Industrial safety standards
- OEE (Overall Equipment Effectiveness)

## Good Luck! 🚀

Remember: In industrial software, **reliability and maintainability** are more important than clever code. Write code that your team can understand and maintain 6 months from now.

---

*This is a practice project. The ESPERA company and LM-3000 machine are fictional elements created for educational purposes.*
