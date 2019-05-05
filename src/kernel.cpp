#include <common/types.h>
#include <gdt.h>
#include <memorymanagement.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <drivers/amd_am79c973.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <multitasking.h>

// #define GRAPHICSMODE

using namespace iregonos;
using namespace iregonos::common;
using namespace iregonos::drivers;
using namespace iregonos::hardwarecommunication;
using namespace iregonos::gui;

void cleanScreen(int length, 
                 int height);

void deleteCharacter(int x, 
                     int y);

void printf(char *str) {
    static uint16_t *videoMemory = (uint16_t *) 0xb8000;
    static uint8_t x = 0, y = 0;

    for (int i = 0; str[i] != '\0'; ++i) {
        switch (str[i]) {
            // Go to new line after \n symbol
            case '\n':
                x = 0;
                y++;
                break;
                // Print character
            default:
                videoMemory[80 * y + x] = (videoMemory[80 * y + x] & 0xFF00) | str[i];
                x++;
                break;
        }

        // Go to new line after line filled
        if (x >= 80) {
            x = 0;
            y++;
        }

        // Clean screen
        if (y >= 25) {
            cleanScreen(80, 25);
            // Reset cursor
            x = 0;
            y = 0;
        }
    }
}

void cleanScreen(int length, 
                 int height) {
    for (int y = 0; y < height; y++)
        for (int x = 0; x < length; x++)
            deleteCharacter(x, y);
}

void deleteCharacter(int x, 
                     int y) {
	static uint16_t *videoMemory = (uint16_t *) 0xb8000;
	
    videoMemory[80 * y + x] = (videoMemory[80 * y + x] & 0xFF00) | ' ';
}

void printfHex(uint8_t key) {
    char *foo = "00";
    char *hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0xF];
    foo[1] = hex[key & 0xF];
    printf(foo);
}

void printfHex16(uint16_t key) {
    printfHex((key >> 8) & 0xFF);
    printfHex( key & 0xFF);
}

void printfHex32(uint32_t key) {
    printfHex((key >> 24) & 0xFF);
    printfHex((key >> 16) & 0xFF);
    printfHex((key >> 8) & 0xFF);
    printfHex( key & 0xFF);
}

class PrintfKeyboardEventHandler : public KeyboardEventHandler {
public:
    void OnKeyDown(char c) {
        char *foo = " ";
        foo[0] = c;
        printf(foo);
    }
};

class MouseToConsole : public MouseEventHandler {
    int8_t x, y;
public:

    MouseToConsole() {
        uint16_t *VideoMemory = (uint16_t *) 0xb8000;
        x = 40;
        y = 12;
        VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0x0F00) << 4
                                  | (VideoMemory[80 * y + x] & 0xF000) >> 4
                                  | (VideoMemory[80 * y + x] & 0x00FF);
    }

    virtual void OnMouseMove(int xoffset, int yoffset) {
        static uint16_t *VideoMemory = (uint16_t *) 0xb8000;
        VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0x0F00) << 4
                                  | (VideoMemory[80 * y + x] & 0xF000) >> 4
                                  | (VideoMemory[80 * y + x] & 0x00FF);

        x += xoffset;
        if (x >= 80) x = 79;
        if (x < 0) x = 0;
        y += yoffset;
        if (y >= 25) y = 24;
        if (y < 0) y = 0;

        VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0x0F00) << 4
                                  | (VideoMemory[80 * y + x] & 0xF000) >> 4
                                  | (VideoMemory[80 * y + x] & 0x00FF);
    }
};

// Task for testing
void taskA() {
    while(true)
        printf("A");
}
void taskB() {
    while(true)
        printf("B");
}

typedef void (*constructor)();

extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors() {
    for (constructor *i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

extern "C" void kernelMain(const void *multiboot_structure, 
                           uint32_t /*multiboot_magic*/) {
    printf("IregonOS\nAlessandro Tornesello --- https://github.com/iregon/IregonOS\n\n");
    
    GlobalDescriptorTable gdt;
    
    uint32_t* memupper = (uint32_t*)(((size_t)multiboot_structure) + 8);
    size_t heap = 10*1024*1024;
    MemoryManager memoryManager(heap, (*memupper)*1024 - heap - 10*1024);
    
    // BEGIN Dynamic memory management
    printf("heap: 0x");
    printfHex((heap >> 24) & 0xFF);
    printfHex((heap >> 16) & 0xFF);
    printfHex((heap >> 8 ) & 0xFF);
    printfHex((heap      ) & 0xFF);

    void* allocated = memoryManager.malloc(1024);
    printf("\nallocated: 0x");
    printfHex(((size_t)allocated >> 24) & 0xFF);
    printfHex(((size_t)allocated >> 16) & 0xFF);
    printfHex(((size_t)allocated >> 8 ) & 0xFF);
    printfHex(((size_t)allocated      ) & 0xFF);
    printf("\n");
    // END
    
    TaskManager taskManager;
    // Task for testing
    /* 
    Task task1(&gdt, taskA);
    Task task2(&gdt, taskB);
    taskManager.AddTask(&task1);
    taskManager.AddTask(&task2);
    */
    
    InterruptManager interrupts(0x20, &gdt, &taskManager);

    printf("Initializing Hardware, Stage 1\n");
    
    #ifdef GRAPHICSMODE
        Desktop desktop(320,200, 0x00,0x00,0xA8);
    #endif

    DriverManager drvManager;

    // Keyboard interrupt handler and driver
    #ifdef GRAPHICSMODE
        KeyboardDriver keyboard(&interrupts, &desktop);
    #else
        PrintfKeyboardEventHandler kbhandler;
        KeyboardDriver keyboard(&interrupts, &kbhandler);
    #endif
        
    drvManager.AddDriver(&keyboard);

    // Mouse interrupt handler and driver
    #ifdef GRAPHICSMODE
        MouseDriver mouse(&interrupts, &desktop);
    #else
        MouseToConsole mousehandler;
        MouseDriver mouse(&interrupts, &mousehandler);
    #endif
        
    drvManager.AddDriver(&mouse);
    
    printf("\n### PCI\n");
    PeripheralComponentInterconnectController PCIController;
    PCIController.SelectDrivers(&drvManager, &interrupts);
    
    VideoGraphicsArray vga;
    
    printf("\nInitializing Hardware, Stage 2\n");
    drvManager.ActivateAll();

    printf("\nInitializing Hardware, Stage 3\n");
    
    #ifdef GRAPHICSMODE
        vga.SetMode(320,200,8);
        
        Window win1(&desktop, 10,10,20,20, 0xA8,0x00,0x00);
        desktop.AddChild(&win1);
        
        Window win2(&desktop, 40,15,30,30, 0x00,0xA8,0x00);
        desktop.AddChild(&win2);
    #endif
    
    // BEGIN Networking
    amd_am79c973* eth0 = (amd_am79c973*)(drvManager.drivers[2]);
    eth0->Send((uint8_t*)"Hello Network", 13);
    // END
    
    interrupts.Activate();
    
    while(1) {
        #ifdef GRAPHICSMODE
            desktop.Draw(&vga);
        #endif
    }
}
