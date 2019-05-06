// For the system call codes see:
// http://shell-storm.org/shellcode/files/syscalls.html

#include <syscalls.h>
 
using namespace iregonos;
using namespace iregonos::common;
using namespace iregonos::hardwarecommunication;
 
SyscallHandler::SyscallHandler(InterruptManager* interruptManager, 
                               uint8_t InterruptNumber)
:    InterruptHandler(interruptManager, InterruptNumber  + interruptManager->HardwareInterruptOffset()) {}

SyscallHandler::~SyscallHandler() {}


void printf(char*);

uint32_t SyscallHandler::HandleInterrupt(uint32_t esp) {
    CPUState* cpu = (CPUState*)esp;
    
    // eax: syscall reference code
    switch(cpu->eax)
    {
        case 4:
            // ebx contains ascii number to print
            printf((char*)cpu->ebx);
            break;
            
        default:
            break;
    }

    return esp;
}
