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
#include <cstdint>

CSE321_LCD LCD(16,2,LCD_5x8DOTS,PF_0,PF_1);  // PF_0 = SDA, PF_1 = SCL

void c1isr(void); // Column 1 Interrupt (1, 4, 7, *)
void c2isr(void); // Column 2 Interrupt (2, 5, 8, 0)
void c3isr(void); // Column 3 Interrupt (3, 6, 9, #)
void c4isr(void); // Column 4 Interrupt (A, B, C, D))

int runNumber(); // See text file for explanation

EventQueue qu(32 * EVENTS_EVENT_SIZE);
Thread t;
int row = 0; // var to use to determine row

// setup interrupt objects
InterruptIn int1(PB_8, PullDown);
InterruptIn int2(PB_9, PullDown);
InterruptIn int3(PB_10, PullDown);
InterruptIn int4(PB_11, PullDown);

int main() {
    
    LCD.begin(); // I am pretty sure this is NOT supposed to be this high, but eh, I'll fix it later.
    LCD.clear();
    LCD.print("test 2");

    t.start(callback(&qu, &EventQueue::dispatch_forever));
    // RCC
    RCC->AHB2ENR |= 6; // Enable ports B + C
    // MODER
    GPIOB->MODER &= ~(0xFF0000); // Set input ports

    GPIOC->MODER &= ~(0xFF0000); // Result: 1111 1111 0000 0000 1111 1111 1111 1111. Guarantees the 0's. 
    GPIOC->MODER |= 0x550000;    // Result: 0000 0000 0101 0101 0000 0000 0000 0000. The OUTPUTS are now what we want.

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

        if (row == 0){
            row = 1;
            GPIOC -> ODR = 0x200; // Enable pin 8 
        }
        else if (row == 1){
            row = 2;
            GPIOC -> ODR = 0x400; // Enable pin 9
        }
        else if (row == 2){
            row = 3;
            GPIOC -> ODR = 0x800; // Enable pin 10
        }
        else if (row == 3){
            row = 0;
            GPIOC -> ODR = 0x100; // Enable pin 11
        }

        // delay
        thread_sleep_for(50); // 50 ms
    }

    return 0;
}

// ISR for C1 - 1, 4, 7, or *
void c1isr(void) {
    if (row == 0) {
        printf("1\n"); 
    } 
    else if (row == 1){
        printf("4\n"); 
    }
    else if (row == 2){
        printf("7\n");
    }
    else if (row == 3) {
        printf("*\n");
    }
    wait_us(500); // 500 us
}

// ISR for C1 - 2, 5, 8, 0
void c2isr(void) {
    if (row == 0) {
        printf("2\n"); 
    } 
    else if (row == 1){
        printf("5\n"); 
    }
    else if (row == 2){
        printf("8\n");
    }
    else if (row == 3) {
        printf("0\n");
    }
    wait_us(500); // 500 us
}

// ISR for C3 - 3, 6, 9, #
void c3isr(void) {
    if (row == 0) {
        printf("3\n"); 
    } 
    else if (row == 1){
        printf("6\n"); 
    }
    else if (row == 2){
        printf("9\n");
    }
    else if (row == 3) {
        printf("#\n");
    }
    wait_us(500); // 500 us
}

// ISR for C4 - A, B, C, D
void c4isr(void) {
    if (row == 0) {
        printf("A\n"); 
    } 
    else if (row == 1){
        printf("B\n");
    }
    else if (row == 2){
        printf("C\n");
    }
    else{
        printf("D\n");
    }
    wait_us(500); // 500 us
}
