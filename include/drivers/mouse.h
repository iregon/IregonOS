#ifndef __IREGONOS__DRIVERS__MOUSE_H
#define __IREGONOS__DRIVERS__MOUSE_H

#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <drivers/driver.h>
#include <hardwarecommunication/interrupts.h>

namespace iregonos {
    namespace drivers {

        class MouseEventHandler {
        public:
            MouseEventHandler();

            virtual void OnActivate();

            virtual void OnMouseDown(iregonos::common::uint8_t button);

            virtual void OnMouseUp(iregonos::common::uint8_t button);

            virtual void OnMouseMove(int x, 
                                     int y);
        };

        class MouseDriver 
        : public iregonos::hardwarecommunication::InterruptHandler, public Driver {
            iregonos::hardwarecommunication::Port8Bit dataport;
            iregonos::hardwarecommunication::Port8Bit commandport;
            iregonos::common::uint8_t buffer[3];
            iregonos::common::uint8_t offset;
            iregonos::common::uint8_t buttons;

            MouseEventHandler *handler;

        public:
            MouseDriver(iregonos::hardwarecommunication::InterruptManager *manager,
                        MouseEventHandler *handler);

            ~MouseDriver();

            virtual iregonos::common::uint32_t HandleInterrupt(iregonos::common::uint32_t esp);

            virtual void Activate();
        };
    }
}

#endif
