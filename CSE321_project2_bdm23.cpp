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

EventQueue qu(32 * EVENTS_EVENT_SIZE);
Thread t;
int row = 0; // var to use to determine row

// setup interrupt objects
InterruptIn int1(PB_8, PullDown);
InterruptIn int2(PB_9, PullDown);

int main() {
    LCD.begin(); // I am pretty sure this is NOT supposed to be this high, but eh, I'll fix it later.
    LCD.clear();
    LCD.print("test 6");

    t.start(callback(&qu, &EventQueue::dispatch_forever));
    // RCC
    RCC->AHB2ENR |= 6;
    // MODER
    GPIOB->MODER &= ~(0xF0000);

    GPIOC->MODER &= ~(0xA0000);
    GPIOC->MODER |= 0x50000;

    GPIOG->MODER |= 5;
    GPIOG->MODER &= ~(0xA);

    RCC->AHB2ENR |= 0x40;
    // set up interrupt behavior

    int1.rise(qu.event(c1isr));
    int2.rise(qu.event(c2isr));

    int1.enable_irq();
    int2.enable_irq();
    while (true) { // need the polling piece

        if (row == 0) {
        row = 1;
        GPIOC->ODR = 0x200;
        } else {
        row = 0;
        GPIOC->ODR = 0x100;
        }
        // delay
        thread_sleep_for(100); // 50 ms
    }

    return 0;
}

// ISR for C1 - 1, 4, 7, or *
void c1isr(void) {
    // which row
    if (row == 0) {
        GPIOG->ODR = 1;
        printf("*\n");
    } 
    else if (row == 1){
        GPIOG->ODR = 0;
        // printf("7\n"); Supposed to print this
        printf("1\n"); // For consistency, leaving this in
    }
    else if (row == 2){
        //GPIOG->ODR=0; I don't know what this does yet. 
        printf("4\n");
    }
    else{
        //GPIOG->ODR = 0; I still don't know what this does.
        printf("1\n");
    }
    wait_us(500); // 500 us
}

// ISR for C2 - 1, 4, 7, or *
void c2isr(void) {
    // which row
    if (row == 0) {
        GPIOG->ODR = 1;
        printf("D\n");
    } 
    else if (row == 1){
        GPIOG->ODR = 0;
        // printf("C\n"); Supposed to print this
        printf("A\n"); // For consistency, leaving this in
    }
    else if (row == 2){
        //GPIOG->ODR=0; I don't know what this does yet. 
        printf("B\n");
    }
    else{
        //GPIOG->ODR = 0; I still don't know what this does.
        printf("A\n");
    }
    wait_us(500); // 500 us
}