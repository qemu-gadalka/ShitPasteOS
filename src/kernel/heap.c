#include "includes/heap.h"

static uint32_t heap_ptr;

void heap_init(uint32_t start) {
    heap_ptr = start;
}

void* kmalloc(uint32_t size) {
    void* addr = (void*)heap_ptr;
    heap_ptr += size;
    return addr;
}
