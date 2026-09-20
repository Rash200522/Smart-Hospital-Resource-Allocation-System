Smart Hospital & Resource Allocation System

CSC 1012 - Introduction to Computer Programming
University of Sri Jayewardenepura - Faculty of Applied Sciences


Project Overview
 The **Smart Hospital & Resource Allocation System** is a modular,
 menu-driven console application developed in **C programming language**. 
 It simulates the daily operations of a hospital, including
   - Patient intake and registration
   - Bed allocation and tracking
   - Emergency triage sorting
   - Medical billing estimations
   - Resource management and reporting

     
This project demonstrates the application of fundamental programming concepts such as:

- 1D and 2D arrays (parallel arrays)
- Custom functions and modular programming
- String handling
- Loops and conditionals
- Mathematical modelling
- Basic file handling

Features
### Core Functionality
   
| Feature | Description |
|---------|-------------|
| **Patient Registration** | Collects name, age, urgency level, specialty, ward admission details |
| **Emergency Triage** | Classifies patients as Normal (1), Urgent (2), or Critical (3) |
| **Bed Management** | 2D array-based tracking with real-time availability |
| **Automated Billing** | Calculates fees, surcharges, and age-based subsidies |
| **Priority Sorting** | Displays patients in triage order (Critical → Urgent → Normal) |
| **Waiting Time Estimation** | Queue-based estimation per specialty |


### Reports & Analytics
     - Bed occupancy status saved between sessions
     - Permanent patient billing logs
     - Automatic data folder creation
    
### Data Persistence
- Bed occupancy status saved between sessions
- Permanent patient billing logs
- Automatic data folder creation

### User Experience
   - Clean, formatted console output
   - Robust input validation
   - Visual bed occupancy bars
   - Clear error messages
     
### System Requirements

### Minimum Requirements

| Component | Requirement |
|-----------|-------------|
| **Operating System** | Windows 7/10/11, Linux, or macOS |
| **Compiler** | GCC (GNU Compiler Collection) |
| **IDE** | Code::Blocks 20.03 or later |
| **RAM** | 512 MB |
| **Disk Space** | 50 MB |
| **Terminal** | ANSI-compatible for formatting |

### Recommended Setup

- **OS:** Windows 10/11 64-bit
- **IDE:** Code::Blocks with MinGW-w64
- **Compiler Flags:** `-Wall -Wextra -std=c99`
- **Terminal:** Windows Terminal or PowerShell

### Installation & Setup
Option 1: Using Code::Blocks (Recommended)

1. **Install Code::Blocks** from [codeblocks.org](https://www.codeblocks.org/downloads/) — select the version with MinGW compiler.

2. **Clone or download** this repository:
   ```bash
   git clone https://github.com/Rash200522/Smart-Hospital-Resource-Allocation-System.git
3.Open the project in Code::Blocks:
 **File → Open → Select SmartHospital.cbp
4.Build the project:
**Press F9 or click the gear icon
5.Run the program:
 **Press F10 or click the play icon

### How to Use

| Option | Function | Description |
|--------|----------|-------------|
| **1** | Register New Patient | Full registration with billing |
| **2** | View by Priority | Emergency triage order |
| **3** | View All Patients | Registration order |
| **4** | Reports & Analytics | Statistics and summaries |
| **5** | Bed Occupancy | Real-time ward status |
| **6** | Save & Exit | Persist data and quit |

