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

CSE321_LCD LCD(16,2,LCD_5x8DOTS,PB_9,PB_8); // PB_9 = SDA, PB_8 = SCL

void c1isr(void);
void c2isr(void);

EventQueue qu(32 * EVENTS_EVENT_SIZE);
Thread t;
int row = 0; // var to use to determine row

// setup interrupt objects
InterruptIn int1(PB_8, PullDown);
InterruptIn int2(PB_9, PullDown);

int main() {

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

  // enable
  //   int1.rise(&c1isr);
  //   int2.rise(&c2isr);

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

// ISR for C1 - 1 or *
void c1isr(void) {
  // which row
  if (row == 0) {
    GPIOG->ODR = 1;
    // qu.call(printf,"*\n");
    printf("*\n");
  } else {
    GPIOG->ODR = 0;
    //  qu.call(printf,"1\n");
    printf("1\n");
  }
  wait_us(500); // 500 us
}

// ISR for C2 - A or D
void c2isr(void) {
  // which row
  if (row == 0) {
    GPIOG->ODR = 2;
    //   qu.call(printf,"D\n");
    printf("D\n");
  } else {
    GPIOG->ODR = 0;
    //    qu.call(printf,"A\n");
    printf("A\n");
  }
  wait_us(500); // 500 us
}

/*
int main()
{
    LCD.begin();
    LCD.clear();
    return 0;
}
*/

/*
Checklist start
        - Follow the flowchart in Documentation.pdf, it summarizes the logic this program should take pretty well
        - Make sure to store the previous number and count repeats, this will make programming the password reset easier
        - Recall that there are two reserved passwords: 9407 and 0000
        - Try not to make the code too ugly
        Checklist end
        - Expected start date (on the programming portion): 10/10, if not that then the 16th
        - Currently working on documentation, so I can't exactly start this part yet
*/