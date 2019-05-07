#ifndef __IREGONOS__COMMON__MEMORY_H
#define __IREGONOS__COMMON__MEMORY_H

namespace iregonos {
    namespace common {
        class MemoryManager {
        public:
            MemoryManager();

            ~MemoryManager();

            void memmove(void *dest, void *src, size_t n);
        };
    }
}

#endif
