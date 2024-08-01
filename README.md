# Process Scheduling Algorithms

## Overview

This project implements various process scheduling algorithms to simulate and analyze CPU scheduling strategies. It encompasses a diverse set of algorithms, including:

- First-Come-First-Serve (FCFS)
- Round Robin (RR)
- Shortest Process Next (SPN)
- Shortest Remaining Time (SRT)
- Highest Response Ratio Next (HRRN)
- Feedback Scheduling (with multiple quantum variations)
- Aging Scheduling

Each algorithm is designed to manage and schedule processes effectively, ensuring an optimal approach to CPU resource allocation based on different criteria.

## Key Features

- **Modular Implementation**: Each scheduling algorithm is implemented as a separate function, facilitating easy experimentation and comparison.
- **Detailed Analysis**: The project calculates various performance metrics such as finish time, turnaround time, and normalized turnaround time for each process.
- **Flexible Input Handling**: Utilizes a parser to handle input data, allowing for versatile testing and customization of process parameters.
- **Visualization**: Provides a timeline visualization of process execution, making it easier to understand the scheduling process.

## Technical Details

- **Languages**: C++
- **Libraries**: Standard Template Library (STL)
- **Data Structures**: Vectors, Queues, Priority Queues, Unordered Maps, Tuples
- **Algorithms Implemented**:
  - **FCFS**: Simple scheduling based on process arrival order.
  - **RR**: Time-sliced scheduling to ensure fairness.
  - **SPN**: Prioritizes processes with the shortest service time.
  - **SRT**: Continuously selects the process with the shortest remaining time.
  - **HRRN**: Uses response ratio to determine scheduling priority.
  - **Feedback Scheduling**: Adjusts priority levels based on feedback and quantum values.
  - **Aging Scheduling**: Increases priority over time to prevent starvation.

## Getting Started

To run the project locally, follow these steps:

1. **Clone the Repository**:
   ```sh
   git clone https://github.com/prabhsingh14/ALGOSCHED.git
   cd ALGOSCHED
   ```

2. **Compile the Code**:
   Ensure you have `g++` installed, then run:
   ```sh
   make
   ```

3. **Run the Application**:
   Execute the compiled binary:
   ```sh
   ./lab4
   ```

## Usage

1. **Input Format**:
   - Provide the required algorithm and process details as specified in the parser.
   - Input should include details for various processes and algorithms in the required format.

2. **Output**:
   - The application will display the scheduling timeline and performance metrics for each process.

## Technical Challenges and Solutions

- **Challenge**: Handling the complexity of multiple scheduling algorithms and ensuring correct implementation.
  **Solution**: Implemented each algorithm modularly and validated through rigorous testing.
  
- **Challenge**: Managing priority levels and ensuring accurate timeline updates.
  **Solution**: Utilized priority queues and efficient sorting techniques to manage process scheduling and update timelines dynamically.

## Contributions

Feel free to contribute by submitting issues, pull requests, or suggestions. Your feedback is valuable!
