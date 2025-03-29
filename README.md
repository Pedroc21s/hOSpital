# hOSpital - Hospital Simulation Application

## Description

hOSpital is a hospital simulation application developed in C, which manages patient admission and medical consultations through cooperative multiprocess operations. The application employs semaphores for process synchronization based on the producer-consumer model, memory management, and inter-process communication (IPC). It includes features such as:

- Temporal logging of operations
- Alarms for periodic system state reporting
- Signal handling for graceful application termination

The system is designed to simulate the admission, processing, and consultation of patients by receptionists and doctors. The program is built using cooperative multiprocessing to handle patient queues and staff assignments.

## Features

- **Synchronization**: Uses semaphores for process synchronization, with a producer-consumer model for managing shared memory.
- **Admission Management**: Patients are admitted, processed by receptionists, and consulted by doctors.
- **Temporal Logging**: Tracks and logs admission times, doctor, patient, and receptionist interactions.
- **Alarms**: Periodic alarms to display the current system state.
- **Signal Handling**: Graceful termination of the program upon receiving an interrupt signal.
- **Statistics**: Generates statistics for the system’s performance.

## Developers

- **Renan Silva**
- **Pedro Correia**
- **Sofia Reis**

## How to Compile

1. Clean up previous builds:
   ```bash
   make clean
2. Compile the project:
   ```bash
   make
   
## How to Run the Project

### 1. Prepare the Configuration File:
Create a configuration file (`config.txt`) inside of textfiles directory with the following format:
  ```txt
max_ads
buffers_size
n_patients
n_receptionists
n_doctors
log_filename
statistics_filename
alarm_time
```
### 2. Run the Project:
  ```bash
     ./hOSpital textfiles/config.txt
   ```
### 3. Available Commands:
Once the simulation starts, you can interact with it using the following commands:
- `ad patient doctor`: To admit a new patient to the hospital (requires patient and doctor IDs).
- `info id`: To check the status of a specific admission (using its ID).
- `status`: To query the current status of the hospital’s data container.
- `help`: To display more information about the available commands.
- `end`: To terminate the simulation and clean up resources.

## License
This project was developed as part of the **Operating Systems** course at the University of Lisbon.






