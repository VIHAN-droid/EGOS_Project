# EGOS Operating System: MLFQ & Shell Extensions 💻⚙️

## Overview
This repository contains a custom fork of the **EGOS Operating System**. It was developed to extend the OS's core functionality by introducing a more sophisticated CPU scheduling algorithm and expanding the utility of its command-line interface. 

This project was completed as a bonus assignment for my Operating Systems course in 2025.

## Key Features

### 1. Multi-Level Feedback Queue (MLFQ) Scheduler
Replaced the default scheduler with a custom **MLFQ scheduler** to improve task management and CPU utilization. 
* **Dynamic Priority:** Adjusts process priority based on their CPU usage and execution behavior.
* **Fairness & Efficiency:** Designed to prevent starvation of low-priority tasks while ensuring I/O-bound processes remain highly responsive.

### 2. Custom Shell Commands
Implemented two custom utilities directly into the EGOS shell:
* `grep`: Searches files or standard input for a specific text pattern.
* `wcl`: A customized word/line count utility to quickly analyze text outputs and file contents.
