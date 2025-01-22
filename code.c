#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4

// Data structures
int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
char resource_names[NUMBER_OF_RESOURCES] = {'A', 'B', 'C', 'D'}; // Resource names

// Function prototypes
bool is_safe_state();
int request_resources(int customer_num, int request[]);
void release_resources(int customer_num, int release[]);
void display_state();
bool detect_deadlock();
void resolve_deadlock();

int main() {
    // Initialize data
    printf("Enter the available resources:\n");
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        printf("Resource %c: ", resource_names[i]);
        if (scanf("%d", &available[i]) != 1) {
            printf("Error: Invalid input for available resources.\n");
            return 1;
        }
    }

    printf("Enter the maximum resources for each customer:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("Customer %d:\n", i);
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("  Max %c: ", resource_names[j]);
            if (scanf("%d", &maximum[i][j]) != 1) {
                printf("Error: Invalid input for maximum resources.\n");
                return 1;
            }
        }
    }

    printf("Enter the allocated resources for each customer:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("Customer %d:\n", i);
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("  Allocated %c: ", resource_names[j]);
            if (scanf("%d", &allocation[i][j]) != 1) {
                printf("Error: Invalid input for allocated resources.\n");
                return 1;
            }

            // Calculate need matrix and ensure no negative values
            need[i][j] = maximum[i][j] - allocation[i][j];
            if (need[i][j] < 0) {
                need[i][j] = 0; // Set negative values to zero
            }
        }
    }

    // Command interface
    char command[3];
    while (1) {
        printf("\nEnter a command (RQ, RL, CS, or exit):\n");
        scanf("%s", command);

        if (strcmp(command, "RQ") == 0) {
            int customer_num, request[NUMBER_OF_RESOURCES];
            printf("Enter customer number: ");
            if (scanf("%d", &customer_num) != 1) {
                printf("Error: Invalid input for customer number.\n");
                continue;
            }

            printf("Enter resource request:\n");
            for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
                printf("  Request %c: ", resource_names[i]);
                if (scanf("%d", &request[i]) != 1) {
                    printf("Error: Invalid input for resource request.\n");
                    break;
                }
            }

            if (request_resources(customer_num, request) == 0) {
                printf("Request granted.\n");
                if (is_safe_state()) {
                    printf("System is in a safe state.\n");
                } else {
                    printf("System is not in a safe state.\n");
                }
            } else {
                printf("Request denied.\n");
            }
        } else if (strcmp(command, "RL") == 0) {
            int customer_num, release[NUMBER_OF_RESOURCES];
            printf("Enter customer number: ");
            if (scanf("%d", &customer_num) != 1) {
                printf("Error: Invalid input for customer number.\n");
                continue;
            }

            printf("Enter resources to release:\n");
            for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
                printf("  Release %c: ", resource_names[i]);
                if (scanf("%d", &release[i]) != 1) {
                    printf("Error: Invalid input for resource release.\n");
                    break;
                }
            }

            release_resources(customer_num, release);
            printf("Resources released.\n");
            if (is_safe_state()) {
                printf("System is in a safe state.\n");
            } else {
                printf("System is not in a safe state.\n");
            }
        } else if (strcmp(command, "CS") == 0) {
            display_state();
            if (detect_deadlock()) {
                printf("Deadlock detected! Resolving...\n");
                resolve_deadlock();
            } else {
                printf("No deadlock detected.\n");
            }
        } else if (strcmp(command, "exit") == 0) {
            break;
        } else {
            printf("Invalid command. Use 'RQ', 'RL', 'CS', or 'exit'.\n");
        }
    }

    return 0;
}

// Function to check if the system is in a safe state
bool is_safe_state() {
    int work[NUMBER_OF_RESOURCES];
    bool finish[NUMBER_OF_CUSTOMERS] = {false};

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        work[i] = available[i];
    }

    do {
        bool found = false;
        for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
            if (!finish[i]) {
                bool can_allocate = true;
                for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                    if (need[i][j] > work[j]) {
                        can_allocate = false;
                        break;
                    }
                }
                if (can_allocate) {
                    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                        work[j] += allocation[i][j];
                    }
                    finish[i] = true;
                    found = true;
                }
            }
        }
        if (!found) {
            break;
        }
    } while (1);

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        if (!finish[i]) {
            return false;
        }
    }

    return true;
}

// Function to request resources
int request_resources(int customer_num, int request[]) {
    if (customer_num < 0 || customer_num >= NUMBER_OF_CUSTOMERS) {
        printf("Error: Invalid customer number.\n");
        return -1;
    }

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (request[i] < 0 || request[i] > need[customer_num][i]) {
            printf("Error: Request exceeds the need or invalid input.\n");
            return -1;
        }
        if (request[i] > available[i]) {
            printf("Error: Not enough available resources.\n");
            return -1;
        }
    }

    // Temporarily allocate resources.
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] -= request[i];
        allocation[customer_num][i] += request[i];
        need[customer_num][i] -= request[i];
    }

    // Check if the state is safe.
    if (!is_safe_state()) {
        printf("Error: Request leads to unsafe state. Rolling back.\n");
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
            available[i] += request[i];
            allocation[customer_num][i] -= request[i];
            need[customer_num][i] += request[i];
        }
        return -1;
    }

    return 0;
}

// Function to release resources
void release_resources(int customer_num, int release[]) {
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (release[i] < 0 || release[i] > allocation[customer_num][i]) {
            printf("Error: Invalid release amount for resource %c.\n", resource_names[i]);
            return;
        }

        allocation[customer_num][i] -= release[i];
        available[i] += release[i];
        need[customer_num][i] += release[i];
    }
}

// Function to display the current system state
void display_state() {
    printf("\nCurrent System State:\n");

    printf("Available resources:\n");
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        printf("%c: %d\n", resource_names[i], available[i]);
    }

    printf("\nMaximum resources:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("Customer %d: ", i);
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("%c: %d ", resource_names[j], maximum[i][j]);
        }
        printf("\n");
    }

    printf("\nAllocated resources:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("Customer %d: ", i);
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("%c: %d ", resource_names[j], allocation[i][j]);
        }
        printf("\n");
    }

    printf("\nNeed resources:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("Customer %d: ", i);
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("%c: %d ", resource_names[j], need[i][j]);
        }
        printf("\n");
    }
}

// Function to detect deadlock
bool detect_deadlock() {
    int work[NUMBER_OF_RESOURCES];
    bool finish[NUMBER_OF_CUSTOMERS] = {false};

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        work[i] = available[i];
    }

    while (1) {
        bool progress = false;
        for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
            if (!finish[i]) {
                bool can_allocate = true;
                for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                    if (need[i][j] > work[j]) {
                        can_allocate = false;
                        break;
                    }
                }
                if (can_allocate) {
                    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                        work[j] += allocation[i][j];
                    }
                    finish[i] = true;
                    progress = true;
                }
            }
        }
        if (!progress) {
            break;
        }
    }

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        if (!finish[i]) {
            return true;
        }
    }
    return false;
}

// Function to resolve deadlock
void resolve_deadlock() {
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        bool deadlocked = detect_deadlock();
        if (!deadlocked) {
            printf("Deadlock resolved.\n");
            return;
        }

        // Preempt resources from the first deadlocked customer
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            available[j] += allocation[i][j];
            need[i][j] += allocation[i][j];
            allocation[i][j] = 0;
        }
        printf("Preempted resources from customer %d.\n", i);
    }

    if (detect_deadlock()) {
        printf("Failed to resolve deadlock.\n");
    } else {
        printf("Deadlock resolved.\n");
    }
}
