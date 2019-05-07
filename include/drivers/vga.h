// For example code and config, see:
// https://files.osdev.org/mirrors/geezer/osd/graphics/modes.c

// For theory, see:
// https://wiki.osdev.org/VGA_Hardware              <--- List of register settings
// http://www.osdever.net/FreeVGA/vga/crtcreg.htm   <--- CRT Controller Registers

#ifndef __IREGONOS__DRIVERS__VGA_H
#define __IREGONOS__DRIVERS__VGA_H

#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <drivers/driver.h>

namespace iregonos {
    namespace drivers {

        class VideoGraphicsArray {
        protected:
            hardwarecommunication::Port8Bit miscPort;

            hardwarecommunication::Port8Bit crtcIndexPort;

            hardwarecommunication::Port8Bit crtcDataPort;

            hardwarecommunication::Port8Bit sequencerIndexPort;

            hardwarecommunication::Port8Bit sequencerDataPort;

            hardwarecommunication::Port8Bit graphicsControllerIndexPort;

            hardwarecommunication::Port8Bit graphicsControllerDataPort;

            hardwarecommunication::Port8Bit attributeControllerIndexPort;

            hardwarecommunication::Port8Bit attributeControllerReadPort;

            hardwarecommunication::Port8Bit attributeControllerWritePort;

            hardwarecommunication::Port8Bit attributeControllerResetPort;

            void WriteRegisters(common::uint8_t *registers);

            common::uint8_t *GetFrameBufferSegment();

            virtual common::uint8_t GetColorIndex(common::uint8_t r,
                                                  common::uint8_t g,
                                                  common::uint8_t b);

            unsigned char g_320x200x256[] = {
                /* MISC */
                0x63,
                /* SEQ */
                0x03, 0x01, 0x0F, 0x00, 0x0E,
                /* CRTC */
                0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
                0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
                0xFF,
                /* GC */
                0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
                0xFF,
                /* AC */
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                0x41, 0x00, 0x0F, 0x00, 0x00
            };

        public:
            VideoGraphicsArray();

            ~VideoGraphicsArray();

            virtual bool SupportsMode(common::uint32_t width,
                                      common::uint32_t height,
                                      common::uint32_t colordepth);

            virtual bool SetMode(common::uint32_t width,
                                 common::uint32_t height,
                                 common::uint32_t colordepth);

            virtual void PutPixel(common::int32_t x,
                                  common::int32_t y,
                                  common::uint8_t r,
                                  common::uint8_t g,
                                  common::uint8_t b);

            virtual void PutPixel(common::int32_t x,
                                  common::int32_t y,
                                  common::uint8_t colorIndex);

            virtual void FillRectangle(common::uint32_t x,
                                       common::uint32_t y,
                                       common::uint32_t w,
                                       common::uint32_t h,
                                       common::uint8_t r,
                                       common::uint8_t g,
                                       common::uint8_t b);
        };
    }
}

#endif 
