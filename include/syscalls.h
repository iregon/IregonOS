#ifndef __IREGONOS__SYSCALLS_H
#define __IREGONOS__SYSCALLS_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <multitasking.h>

namespace iregonos {
    
    class SyscallHandler : public hardwarecommunication::InterruptHandler {
    public:
        SyscallHandler(hardwarecommunication::InterruptManager* interruptManager, 
                       iregonos::common::uint8_t InterruptNumber);
        
        ~SyscallHandler();
        
        virtual iregonos::common::uint32_t HandleInterrupt(iregonos::common::uint32_t esp);
    };
}

#endif
