#ifndef __IREGONOS__DRIVERS__DRIVER_H
#define __IREGONOS__DRIVERS__DRIVER_H

namespace iregonos {
    namespace drivers {

        class Driver {
        public:
            Driver();

            ~Driver();

            virtual void Activate();

            virtual int Reset();

            virtual void Deactivate();
        };

        class DriverManager {
        public:
            Driver *drivers[255];
            int numDrivers;

        public:
            DriverManager();

            void AddDriver(Driver *);

            void ActivateAll();

        };
    }
}

#endif
