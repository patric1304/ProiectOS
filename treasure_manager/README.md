# Treasure Hunt Game System

## Overview
The Treasure Hunt Game System is a digital platform that allows users to create, manage, and participate in treasure hunts. Users can add, list, view, and remove treasures, as well as log their activities for future reference.

## Features
- Create and manage treasure hunts
- Add, list, view, and remove treasures
- File operations for managing treasure data
- Logging of user actions and system events

## Project Structure
```
treasure_manager
├── src
│   ├── main.c               # Entry point of the program
│   ├── treasure_hunt.c      # Functions for managing treasure hunts
│   ├── treasure_hunt.h      # Header file for treasure_hunt.c
│   ├── file_operations.c     # File operations for treasure data
│   ├── file_operations.h     # Header file for file_operations.c
│   ├── logger.c              # Logging functionality
│   ├── logger.h              # Header file for logger.c
│   └── utils
│       ├── utils.c          # Utility functions
│       └── utils.h          # Header file for utils.c
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

2. Compile the program using the Makefile:
   ```
   make
   ```

3. Run the program:
   ```
   ./src/main
   ```

## Usage
- Follow the on-screen prompts to create and manage treasure hunts.
- Use command-line arguments to specify actions such as adding or removing treasures.

## Logging
All user actions are logged in `logs/treasure_manager.log` for tracking and debugging purposes.

## Contributing
Contributions are welcome! Please submit a pull request or open an issue for any suggestions or improvements.