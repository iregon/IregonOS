#ifndef __iregonos__DRIVERS__KEYBOARD_H
#define __iregonos__DRIVERS__KEYBOARD_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/driver.h>
#include <hardwarecommunication/port.h>

namespace iregonos
{
    namespace drivers
    {
        class KeyboardEventHandler {
        public:
            KeyboardEventHandler();

            virtual void OnKeyDown(char);
            virtual void OnKeyUp(char);
        };

        class KeyboardDriver : public iregonos::hardwarecommunication::InterruptHandler, public Driver {
            iregonos::hardwarecommunication::Port8Bit dataport;
            iregonos::hardwarecommunication::Port8Bit commandport;
            
            KeyboardEventHandler* handler;
            
        public:
            KeyboardDriver(iregonos::hardwarecommunication::InterruptManager* manager, KeyboardEventHandler *handler);
            ~KeyboardDriver();
            virtual iregonos::common::uint32_t HandleInterrupt(iregonos::common::uint32_t esp);
            virtual void Activate();
        };
    }
}
 #endif 
