# Treasure Hunt Game System

## Overview
The Treasure Hunt Game System is a digital platform that allows users to create, manage, and participate in treasure hunts. The system is split into two main executables: `treasure_hub` (the command interface) and `monitor` (the output and management process). Users interact with the hub to send commands, which are processed and displayed by the monitor.

## Features
- Create and manage treasure hunts
- Add, list, view, and remove treasures
- Add users to treasures
- File operations for managing treasure data
- Logging of user actions and system events
- Monitor process for handling and displaying results
- Inter-process communication via named pipes (FIFO)

## Project Structure
```
treasure_manager
├── src
│   ├── main.c                # (Optional) Entry point for legacy CLI
│   ├── treasure_hub.c        # Main command interface (treasure_hub executable)
│   ├── monitor.c             # Monitor process (monitor executable)
│   ├── treasure_manager.c    # Core logic for managing hunts and treasures
│   ├── treasure_manager.h    # Header for treasure_manager.c
│   ├── file_operations.c     # File operations for treasure data
│   ├── file_operations.h     # Header for file_operations.c
│   ├── logger.c              # Logging functionality
│   ├── logger.h              # Header for logger.c
│   └── utils
│       ├── utils.c           # Utility functions
│       └── utils.h           # Header for utils.c
├── Makefile                  # Build instructions
├── README.md                 # Project documentation
└── logs
    └── treasure_manager.log  # Log file for operations
```

## Setup Instructions
1. Clone the repository:
   ```
   git clone <repository-url>
   cd treasure_manager
   ```

2. Compile the programs using the Makefile:
   ```
   make
   ```

   This will produce two executables: `treasure_hub` and `monitor` in the `src/` directory.

3. Create the named pipe (FIFO) for monitor communication if it does not exist:
   ```
   mkfifo monitor_pipe
   ```

4. (Recommended) Open two terminals:
   - **Terminal 1:** Start the monitor process:
     ```
     ./src/monitor
     ```
   - **Terminal 2:** Start the treasure hub interface:
     ```
     ./src/treasure_hub
     ```

   The hub sends commands to the monitor, and the monitor displays results.

## Usage
- In the `treasure_hub` terminal, type commands such as `start_monitor`, `stop_monitor`, `create_hunt`, `add_treasure`, `list_hunts`, `list_treasures`, `view_treasure`, `add_user_to_treasure`, etc.
- Most command results will appear in the monitor terminal.
- Use `help` in the hub for a list of available commands.

## Logging
All user actions and system events are logged in `logs/treasure_manager.log` for tracking and debugging purposes.

## Contributing
Contributions are welcome! Please submit a pull request or open an issue for any suggestions or improvements.