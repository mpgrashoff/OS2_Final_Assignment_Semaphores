/**
 * @author Marijn Grashoff
 * @date 28-06-2023
 * @file supermarket_simulation.c
 * @brief A simulation of a supermarket checkout system with threads and semaphores.
 * @note most of the doxygen documentation was written by ChatGPT
 */

#include <stdio.h>
#include "stdlib.h"
#include <stdbool.h>
#include "semaphore.h"
#include <pthread.h>
#include "time.h"

#define Simulation_length 300 /**< Length of simulation in number of customers */
#define Carts 40 /**< Total number of shopping carts available */
#define Scanners 10 /**< Total number of handheld scanners available */
#define Cashiers 3 /**< Number of cashiers */
#define Terminals 3 /**< Number of checkout terminals */
#define story false /**< If true, prints additional simulation narrative if false activates the info thread showing real time data of the store simulation */

sem_t processed_customers; /**< Semaphore to track processed customers */
sem_t available_carts; /**< Semaphore to track available shopping carts */
sem_t available_scanners; /**< Semaphore to track available handheld scanners */
sem_t cashier_queue_lengths[Cashiers]; /**< Semaphores to track queue lengths at each cashier */
sem_t cashier_access[Cashiers]; /**< Semaphores to control access to each cashier */
sem_t terminal_queue; /**< Semaphore to control access to checkout terminals */
pthread_mutex_t queue_locks[Cashiers]; /**< Mutexes to lock access to each cashier's queue */
pthread_mutex_t terminal_lock; /**< Mutex to lock access to the checkout terminal */

/**
 * @brief Delay function to pause execution for a given number of seconds.
 * @param seconds Number of seconds to delay.
 */
void delay(long seconds) {
    struct timespec ts;
    ts.tv_sec = seconds;
    ts.tv_nsec = 0;
    if (nanosleep(&ts, NULL) < 0) {
        perror("nanosleep");
    }
}

/**
 * @brief Information thread function that prints current simulation state periodically.
 * @if only works when Story is set to false
 */
void *INFO()
{
    for (;;) {
        int val;
        sem_getvalue(&available_carts, &val);
        printf("\nNumber of carts available: %d\n", val);

        sem_getvalue(&available_scanners, &val);
        printf("Number of scanners available: %d\n", val);

        for (int i = 0; i < Cashiers; i++) {
            sem_getvalue(&cashier_queue_lengths[i], &val);
            printf("Queue length at cashier %d: %d\n", i + 1, val);
        }

        sem_getvalue(&terminal_queue, &val);
        printf("Queue length at terminal: %d\n", Terminals - val);

        sem_getvalue(&processed_customers, &val);
        printf("Number of processed customers: %d\n", val);

        delay(2); // Delay 2 seconds
    }
}

/**
 * @brief Customer thread function simulating shopping and checkout process.
 * @param arg Pointer to customer ID (integer).
 */
void *THR_Customer(void *arg)
{
    int customer_id = *(int *)arg;
    bool has_scanner = false;

    if (sem_trywait(&available_carts) != 0) {
        if (story) printf("%d No carts available. Customer leaves.\n", customer_id);
        pthread_exit(NULL);
    }

    if (story) printf("Hello, new customer! %d with cart\n", customer_id);

    // Try to take a handheld scanner
    if (rand() % 2 == 1 && Terminals !=0 && sem_trywait(&available_scanners) == 0 ) {
        has_scanner = true;
        if (story) printf("%d took a handheld scanner.\n", customer_id);
    } else {
        if (story) printf("No handheld scanner available, or customer chose not to take one.\n");
    }

    delay((rand() % 10) + 1); // Simulate shopping time

    if (story) printf("%d done shopping going to checkout\n", customer_id);

    if (has_scanner) {
        if (story) printf("%d Customer with handheld scanner going to checkout terminal.\n", customer_id);

        sem_wait(&terminal_queue);
        pthread_mutex_lock(&terminal_lock);
        if (story) printf("%d Customer at terminal.\n", customer_id);

        if (rand() % 4 == 0) delay(rand() % 11 + 9); // Simulate a bag check
        delay(1); // Registering and paying takes 1 second

        pthread_mutex_unlock(&terminal_lock);
        sem_post(&terminal_queue);
        sem_post(&available_scanners); // Scanner becomes available again

        if (story) printf("%d Customer done at terminal.\n", customer_id);
    }
    else {
        if (story) printf("%d Customer going to a cashier.\n", customer_id);

        // Find the shortest cashier queue
        int min_queue_index = 0;
        int min_queue_value;
        sem_getvalue(&cashier_queue_lengths[min_queue_index], &min_queue_value);

        for (int i = 1; i < Cashiers; ++i) {
            int value;
            sem_getvalue(&cashier_queue_lengths[i], &value);
            if (value < min_queue_value) {
                min_queue_value = value;
                min_queue_index = i;
            }
        }

        sem_post(&cashier_queue_lengths[min_queue_index]); // Join the shortest queue
        sem_wait(&cashier_access[min_queue_index]); // Wait for the cashier to be available
        pthread_mutex_lock(&queue_locks[min_queue_index]); // Lock the cashier for payment

        if (story) printf("%d Customer at cashier %d.\n", customer_id, min_queue_index);

        delay((rand() % 8 + 3)); // Registering and paying time

        pthread_mutex_unlock(&queue_locks[min_queue_index]); // Unlock the cashier after payment
        sem_post(&cashier_access[min_queue_index]);
        sem_wait(&cashier_queue_lengths[min_queue_index]); // Leave the queue

        if (story) printf("%d Customer done at cashier.\n", customer_id);
    }

    if (story) printf("%d leaving store\n", customer_id);

    delay((rand() % 2) + 1); // Delay before returning the cart
    sem_post(&available_carts);

    if (story) printf("cart returned\n");

    sem_post(&processed_customers);
    pthread_exit(NULL);
}

/**
 * @brief Function to start the supermarket simulation.
 */
void run()
{
    int customer_id = 1;
    pthread_t threads[Carts + 5]; // Assuming threads terminate when no carts are available, with a buffer of 5
    pthread_t info_thread;
    if (!story) pthread_create(&info_thread, NULL, INFO, NULL);

    while (customer_id <= Simulation_length) {
        delay((rand() % 2 + 1)); // New customer every 2 to 4 seconds

        pthread_create(&threads[customer_id - 1], NULL, THR_Customer, &customer_id);
        pthread_detach(threads[customer_id - 1]);

        customer_id++;
    }

    if (!story) pthread_join(info_thread, NULL);

    sem_destroy(&available_carts);
    sem_destroy(&available_scanners);
    sem_destroy(&processed_customers);

    for (int i = 0; i < Cashiers; ++i) {
        sem_destroy(&cashier_queue_lengths[i]);
        pthread_mutex_destroy(&queue_locks[i]);
    }

    sem_destroy(&terminal_queue);
    pthread_mutex_destroy(&terminal_lock);
}

/**
 * @brief Main function, entry point of the program.
 * @return 0 on success.
 */
int main(void) {
    srand(time(NULL)); // Seed random number generator
    sem_init(&processed_customers, 0, 0); // Initialize processed customers count to 0
    sem_init(&available_carts, 0, Carts);
    sem_init(&available_scanners, 0, Scanners);

    for (int i = 0; i < Cashiers; ++i) {
        sem_init(&cashier_queue_lengths[i], 0, 0); // Initialize queue length to 0
        sem_init(&cashier_access[i], 0, 1); // Initialize cashier access to 1
        pthread_mutex_init(&queue_locks[i], NULL);
    }

    sem_init(&terminal_queue, 0, Terminals);
    pthread_mutex_init(&terminal_lock, NULL);

    run();

    return 0;
}
