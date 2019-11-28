#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stdbool.h>

void memory_test(void);

void memory_initialize(void);

uint32_t memory_available(void);

uint32_t memory_used(void);

void *memory_allocate(uint32_t size);

bool memory_release(void *ptr);

#endif
