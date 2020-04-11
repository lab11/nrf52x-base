#ifndef VIRTUAL_TIMER_H_
#define VIRTUAL_TIMER_H_
#pragma once

#include "nrf.h"

// Type for the function pointer to call when the timer expires
typedef void (*virtual_timer_callback_t)(void);

// Read the current value of the hardware timer counter
// Returns the counter value
uint32_t read_timer(void);

// Initialize the timer peripheral
void virtual_timer_init(void);

// Start a one-shot timer that calls <cb> <microseconds> in the future
// Returns a unique timer_id
uint32_t virtual_timer_start(uint32_t microseconds, virtual_timer_callback_t cb);

// Start timer that repeatedly calls <cb> <microseconds> in the future
// Returns a unique timer_id
uint32_t virtual_timer_start_repeated(uint32_t microseconds, virtual_timer_callback_t cb);

// Takes a timer_id and cancels that timer such that it stops firing
void virtual_timer_cancel(uint32_t timer_id);


 #endif