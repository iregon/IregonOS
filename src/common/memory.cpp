#include <memory.h>

MemoryManager::MemoryManager() {}

MemoryManager::~MemoryManager() {}

void MemoryManager::memmove(void *dest, void *src, size_t n) {
    for (int i = 0; i < n; i++)
        dest[i] = src[i];
}