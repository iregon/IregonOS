#ifndef __iregonos__GDT_H
#define __iregonos__GDT_H

#include <common/types.h>

namespace iregonos {

    class GlobalDescriptorTable {
    public:

        class SegmentDescriptor {
        private:
            iregonos::common::uint16_t limit_lo;
            iregonos::common::uint16_t base_lo;
            iregonos::common::uint8_t base_hi;
            iregonos::common::uint8_t type;
            iregonos::common::uint8_t limit_hi;
            iregonos::common::uint8_t base_vhi;

        public:
            SegmentDescriptor(iregonos::common::uint32_t base, iregonos::common::uint32_t limit, iregonos::common::uint8_t type);
            iregonos::common::uint32_t Base();
            iregonos::common::uint32_t Limit();
        } __attribute__((packed));

        private:
        SegmentDescriptor nullSegmentSelector;
        SegmentDescriptor unusedSegmentSelector;
        SegmentDescriptor codeSegmentSelector;
        SegmentDescriptor dataSegmentSelector;

        public:

        GlobalDescriptorTable();
        ~GlobalDescriptorTable();

        iregonos::common::uint16_t CodeSegmentSelector();
        iregonos::common::uint16_t DataSegmentSelector();
    };

 }

 #endif
