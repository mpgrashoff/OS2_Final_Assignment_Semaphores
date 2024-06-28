# Supermarket Checkout Simulation

## Overview

This project simulates a supermarket checkout system using threads and semaphores in C. The simulation models customer behavior as they shop, choose between cashiers or handheld scanners, and complete the checkout process. The objective is to learn how to write a concurrent application in C using semaphores.

## Problem Statement

### Operating Systems 2 - Assignment Semaphores - 2024

**Learning Objective:**  
In this assignment, you learn how to write a concurrent application in C by means of semaphores.

**Case: Supermarket Checkout**  
In this made-up scenario, we model the behavior of how customers can pay and checkout their groceries in a supermarket. Customers have the choice between multiple cashiers or using a handheld scanner, which can be quicker than waiting in line by a cashier.

## Assumptions and Requirements

- Every 2 to 4 seconds, a customer arrives and takes a cart to go shopping. There are 20 carts available. If no cart is available, the customer leaves.
- About half the customers take a handheld scanner with them. There are 10 handheld scanners available. If none are available, the customer shops the normal way.
- Shopping takes between 1 to 10 seconds.
- Customers without a handheld scanner go to one of three cashiers. They choose the cashier with the shortest queue.
- Customers with a handheld scanner go to one of two checkout terminals. Registering and paying at a terminal takes 1 second.
- After paying, customers take 1 or 2 seconds to return their cart and leave.

## Features

- Simulation of customer arrival, shopping, and checkout.
- Use of semaphores to manage shared resources like carts, scanners, cashiers, and terminals.
- Display of current simulation state, including the number of available carts, scanners, and queue lengths.

## Additional Features (Optional for Higher Grade)

1. **Random Bag Checks:** A 25% chance that a customer with a handheld scanner is checked, taking significantly longer.
2. **Scanner Location Management:** Handheld scanners are picked up and returned at different locations. // not implemented
3. **Manual Checkout:** Customers can choose manual checkout at terminals if cashier queues are long, with a 25% chance of a random check. // not implemented

## Implementation

### Threads

- **Customer Thread (`THR_Customer`)**: Simulates a customer's shopping and checkout process.
- **Information Thread (`INFO`)**: Periodically displays the current state of the simulation.

### Semaphores and Mutexes

- **Carts and Scanners**: Semaphores to manage the availability of carts and scanners.
- **Cashier Queues and Terminals**: Semaphores and mutexes to manage access to cashiers and checkout terminals.

### Functions

- **`delay`**: Simulates time passing.
- **`THR_Customer`**: Simulates customer behavior.
- **`INFO`**: Displays current simulation state.
- **`run`**: Starts the simulation.
- **`main`**: Initializes resources and starts the simulation.

## Usage

1. **Compile the Code**:
   ```bash
   gcc -o supermarket_simulation supermarket_simulation.c -lpthread
   ```

2. **Run the Simulation**:
   ```bash
   ./supermarket_simulation
   ```

## Testing

- Test by changing the number of carts, scanners, cashiers, and terminals.
- Randomize the time frequencies for customer arrival, shopping, and checkout.
- Use screenshots to show different scenarios and behaviors.

## Author

This project was created by MPGrashoff. You can find more of my work on GitHub: [MPGrashoff](https://github.com/MPGrashoff).

## Conclusion

This project demonstrates the use of semaphores in a concurrent application. By simulating a supermarket checkout system, it showcases how to manage shared resources and synchronize activities using semaphores.



This README provides an overview of the project, instructions for running the simulation, and additional context for the assignment. It helps users and reviewers understand the project's purpose, usage, and implementation details.