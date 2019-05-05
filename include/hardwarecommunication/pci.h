#ifndef __IREGONOS__HARDWARECOMMUNICATION__PCI_H
#define __IREGONOS__HARDWARECOMMUNICATION__PCI_H

#include <hardwarecommunication/port.h>
#include <drivers/driver.h>
#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <memorymanagement.h>

namespace iregonos {
    namespace hardwarecommunication {
        enum BaseAddressRegisterType {
            MemoryMapping = 0,
            InputOutput = 1
        };

        class BaseAddressRegister {
        public:
            bool prefetchable;
            iregonos::common::uint8_t *address;
            iregonos::common::uint32_t size;
            BaseAddressRegisterType type;
        };

        class PeripheralComponentInterconnectDeviceDescriptor {
        public:
            iregonos::common::uint32_t portBase;
            iregonos::common::uint32_t interrupt;

            iregonos::common::uint16_t bus;
            iregonos::common::uint16_t device;
            iregonos::common::uint16_t function;

            iregonos::common::uint16_t vendor_id;
            iregonos::common::uint16_t device_id;

            iregonos::common::uint8_t class_id;
            iregonos::common::uint8_t subclass_id;
            iregonos::common::uint8_t interface_id;

            iregonos::common::uint8_t revision;

            PeripheralComponentInterconnectDeviceDescriptor();

            ~PeripheralComponentInterconnectDeviceDescriptor();

        };


        class PeripheralComponentInterconnectController {
            Port32Bit dataPort;
            Port32Bit commandPort;

        public:
            PeripheralComponentInterconnectController();

            ~PeripheralComponentInterconnectController();

            iregonos::common::uint32_t Read(iregonos::common::uint16_t bus,
                                            iregonos::common::uint16_t device,
                                            iregonos::common::uint16_t function,
                                            iregonos::common::uint32_t registeroffset);

            void Write(iregonos::common::uint16_t bus,
                       iregonos::common::uint16_t device,
                       iregonos::common::uint16_t function,
                       iregonos::common::uint32_t registeroffset,
                       iregonos::common::uint32_t value);

            bool DeviceHasFunctions(iregonos::common::uint16_t bus,
                                    iregonos::common::uint16_t device);

            void SelectDrivers(iregonos::drivers::DriverManager *driverManager,
                               iregonos::hardwarecommunication::InterruptManager *interrupts);

            iregonos::drivers::Driver *GetDriver(PeripheralComponentInterconnectDeviceDescriptor dev,
                                                 iregonos::hardwarecommunication::InterruptManager *interrupts);

            PeripheralComponentInterconnectDeviceDescriptor GetDeviceDescriptor(iregonos::common::uint16_t bus,
                                                                                iregonos::common::uint16_t device,
                                                                                iregonos::common::uint16_t function);

            BaseAddressRegister GetBaseAddressRegister(iregonos::common::uint16_t bus,
                                                       iregonos::common::uint16_t device,
                                                       iregonos::common::uint16_t function,
                                                       iregonos::common::uint16_t bar);
        };
    }
}

#endif 
