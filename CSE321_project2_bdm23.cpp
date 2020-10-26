/*
File: CSE321_project2_bdm23.cpp

Author1: Ben Miller (bdm23@buffalo.edu)
Date: Fall 2020
Partner: I worked alone
Course: CSE321

Summary of File:
    
    Purpose: This code is the implementation of a security system lock. The lock should lock/unlock based on a 4 digit code (9407).
    Modules/Subroutines: None presently. Will update if this changes.
    Assignment: Corresponds to Project 2.
    Input(s): A matrix keypad.
    Output(s): An LED; an LCD display. 
    Constraint(s): Must run indefinitely, must have a response if an incorrect password is given, must "lock" and "unlock"
    Source references: Project PDF file, included in this folder
    
*/


#include "mbed.h"
#include "1802.h"
#include <string.h> 

CSE321_LCD LCD(16,2,LCD_5x8DOTS,PF_0,PF_1);  // Initialize LCD display, PF_0 = SDA, PF_1 = SCL

void c1isr(void); // Column 1 Interrupt (1, 4, 7, *)
void c2isr(void); // Column 2 Interrupt (2, 5, 8, 0)
void c3isr(void); // Column 3 Interrupt (3, 6, 9, #)
void c4isr(void); // Column 4 Interrupt (A, B, C, D))

void update(char *array, char input); // This will be used to update the current password.
char password[4] = ""; // Store the password in here.

DigitalOut blueLED(PB_7); // Set blue user LED as an output
DigitalOut redLED(PB_14); // Set red user LED as an output

EventQueue qu(32 * EVENTS_EVENT_SIZE); // Set up EventQueue
Thread t; // Initialize thread to use with queue
int row = 0; // var to use to determine row

// setup interrupt objects
InterruptIn int1(PB_8, PullDown);
InterruptIn int2(PB_9, PullDown);
InterruptIn int3(PB_10, PullDown);
InterruptIn int4(PB_11, PullDown);

int main() {
    LCD.begin(); // This is always the first thing you do when using the LCD.
    LCD.print("Locked."); // The system begins locked.

    t.start(callback(&qu, &EventQueue::dispatch_forever)); // This allows the thread to indefinitely run.
    // RCC
    RCC->AHB2ENR |= 6; // Enable ports B + C
    // MODER
    GPIOB->MODER &= ~(0xFF0000); // The INPUTS are now properly set.

    GPIOC->MODER &= ~(0xFF0000); // Result: 1111 1111 0000 0000 1111 1111 1111 1111. Guarantees the 0's. 
    GPIOC->MODER |= 0x550000;    // Result: 0000 0000 0101 0101 0000 0000 0000 0000. The OUTPUTS are now properly set.

    // set up interrupt behavior
    int1.rise(qu.event(c1isr));
    int2.rise(qu.event(c2isr));
    int3.rise(qu.event(c3isr));
    int4.rise(qu.event(c4isr));

    int1.enable_irq();
    int2.enable_irq();
    int3.enable_irq();
    int4.enable_irq();

    while (true) { // need the polling piece

        if (row == 0){ // Unfortunately, the pins are out of order. I couldn't find a way to fix this and it works, so I left it as is. 
            row = 1; // Top row
            GPIOC -> ODR = 0x200; // Enable pin 9 i.e. PC9
        }
        else if (row == 1){
            row = 2; // Second-to-top row
            GPIOC -> ODR = 0x400; // Enable pin 10 i.e. PC10
        }
        else if (row == 2){
            row = 3; // Second-to-bottom row
            GPIOC -> ODR = 0x800; // Enable pin 11 i.e. PC11
        }
        else if (row == 3){
            row = 0; // Bottom row
            GPIOC -> ODR = 0x100; // Enable pin 8 i.e. PC8
        }

        // delay
        thread_sleep_for(300); // Turn off thread for 300ms. This SOUNDS high, but it was the best balance I could find between less bounces and user input speed. 
                               // Of course, if you try to rapidly insert numbers it doesn't work. That's the tradeoff being made here. 
    }

    return 0;
}

void update(char *array, char input){ // This is the function that stores and updates the password. 
    strncat(array, &input, 1); // Add the current value to the end of the password.
    if (strlen(array) == 4){ // After four values have been inserted, check the password's validity. 
        if (strcmp(array, "4920") == 0){ // If password is correct...
            LCD.clear(); // Clear the display.
            LCD.print("Unlocked."); // Unlock the system.
            blueLED.write(1); // Turn on the blue LED.
            redLED.write(0); // Turn off the red LED.
        }
        else { // Else, password is not correct.
            LCD.clear(); // Clear the display.
            LCD.print("Locked."); // Lock the sytem.
            LCD.setCursor(0, 1); // Go to second line.
            LCD.print("Incorrect code."); // Just a short line saying "your last input was incorrect." KIND OF redundant, but ¯\_(ツ)_/¯
            redLED.write(1); // Turn on the red LED.
            blueLED.write(0); // Turn off the blue LED.
        }
        strcpy(array, ""); // Clear password so that we can start over. 
    }
}

void c1isr(void) { // ISR for C1 - 1, 4, 7, or *
    if (row == 0) {
        printf("1\n"); // Prints 1 to serial out, used for testing
        update(password, '1'); // User inputted a 1, add to password
    } 
    else if (row == 1){
        printf("4\n");  // Prints 4 to serial out, used for testing
        update(password, '4'); // User inputted a 4, add to password
    }
    else if (row == 2){
        printf("7\n"); // Prints 7 to serial out, used for testing
        update(password, '7'); // User inputted a 7, add to password
    }
    else if (row == 3) {
        printf("*\n"); // Prints * to serial out, used for testing
                       // * serves no purpose in the security system, so nothing else is done
    }
    wait_us(500); // Wait for 500 us
}

void c2isr(void) { // ISR for C1 - 2, 5, 8, 0
    if (row == 0) {
        printf("2\n"); // Prints 2 to serial out, used for testing
        update(password, '2'); // User inputted a 2, add to password
    } 
    else if (row == 1){
        printf("5\n"); // Prints 5 to serial out, used for testing
        update(password, '5'); // User inputted a 5, add to password
    }
    else if (row == 2){
        printf("8\n"); // Prints 8 to serial out, used for testing
        update(password, '8'); // User inputted a 8, add to password
    }
    else if (row == 3) {
        printf("0\n"); // Prints 0 to serial out, used for testing
        update(password, '0'); // User inputted a 0, add to password
    }
    wait_us(500); // Wait for 500 us
}

void c3isr(void) { // ISR for C3 - 3, 6, 9, #
    if (row == 0) {
        printf("3\n"); // Prints 3 to serial out, used for testing
        update(password, '3'); // User inputted a 3, add to password
    } 
    else if (row == 1){
        printf("6\n"); // Prints 6 to serial out, used for testing
        update(password, '6'); // User inputted a 6, add to password
    }
    else if (row == 2){
        printf("9\n"); // Prints 9 to serial out, used for testing
        update(password, '9'); // User inputted a 9, add to password
    }
    else if (row == 3) {
        printf("#\n"); // Prints # to serial out, used for testing
                       // # serves no purpose in the security system, so nothing else is done
    }
    wait_us(500); // Wait for 500 us
}

void c4isr(void) { // ISR for C4 - A, B, C, D
    if (row == 0) {
        printf("A\n"); // Prints A to serial out, used for testing
        strcpy(password, ""); // Reset button hit! Clear password so that the user can start over.
    } 
    else if (row == 1){
        printf("B\n"); // Prints B to serial out, used for testing
                       // B serves no purpose in the security system, so nothing else is done
    }
    else if (row == 2){
        printf("C\n"); // Prints C to serial out, used for testing
                       // C serves no purpose in the security system, so nothing else is done
    }
    else{
        printf("D\n"); // Prints D to serial out, used for testing
                       // D serves no purpose in the security system, so nothing else is done
    }
    wait_us(500); // Wait for 500 us
}



/* 
Was that too much commenting?
...
...
...
It felt like too much commenting.
*/