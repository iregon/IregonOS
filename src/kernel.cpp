#include <common/types.h>
#include <gdt.h>
#include <memorymanagement.h>
#include <hardwarecommunication/interrupts.h>
#include <syscalls.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <drivers/ata.h>
#include <drivers/amd_am79c973.h>
#include <net/etherframe.h>
#include <net/arp.h>
#include <net/ipv4.h>
#include <net/icmp.h>
#include <net/udp.h>
#include <net/tcp.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <multitasking.h>

//#define GRAPHICSMODE

using namespace iregonos;
using namespace iregonos::common;
using namespace iregonos::drivers;
using namespace iregonos::hardwarecommunication;
using namespace iregonos::gui;
using namespace iregonos::net;

void scrollScreen(int width,
                  int height);

void memmove(uint16_t *dest,
             uint16_t *src,
             size_t n);

void cleanScreen(int width,
                 int height);

void deleteLastRow(int numCols,
                   int numRows);

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
                //videoMemory[80 * y + x] = (str[i] >= 0x20 && str[i] <= 0x7e) ? (videoMemory[80 * y + x] & 0xFF00) | str[i] : 0x20;
                videoMemory[80 * y + x] = (str[i] >= 0x20 && str[i] <= 0x7e) ? (videoMemory[80 * y + x] & 0xFF00) | str[i] : (videoMemory[80 * y + x] & 0xFF00) | 0x20;
                x++;
                break;
        }

        // Go to new line after line filled
        if (x >= 80) {
            x = 0;
            y++;
        }

        // TODO fix mouse cursor on scrolling
        if (y >= 25) {
            // Scroll
            scrollScreen(80, 25);
            deleteLastRow(80, 25);
            y = 24;
//            cleanScreen(80, 25);
//            // Reset cursor
//            x = 0;
//            y = 0;
        }
    }
}

void scrollScreen(int width,
                  int height) {
    static uint16_t *videoMemory = (uint16_t *) 0xb8000;

    memmove(videoMemory, &videoMemory[width], 1920);
}

void memmove(uint16_t *dest, uint16_t *src, size_t n) {
    for (int i = 0; i < n; i++)
        dest[i] = src[i];
}

void cleanScreen(int width,
                 int height) {
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            deleteCharacter(x, y);
}

void deleteLastRow(int numCols, int numRows) {
    static uint16_t *videoMemory = (uint16_t *) 0xb8000;
    uint16_t *start = &videoMemory[1920]; // 80 (cols) * 24 (rows)

    for (int i = 0; i < numCols; i++)
        deleteCharacter(i, numRows - 1);
}

void deleteCharacter(int x,
                     int y) {
    static uint16_t *videoMemory = (uint16_t *) 0xb8000;

    // 0x20 = space character code in ascii
    videoMemory[80 * y + x] = (videoMemory[80 * y + x] & 0xFF00) | 0x20;
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
    printfHex(key & 0xFF);
}

void printfHex32(uint32_t key) {
    printfHex((key >> 24) & 0xFF);
    printfHex((key >> 16) & 0xFF);
    printfHex((key >> 8) & 0xFF);
    printfHex(key & 0xFF);
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

class PrintfUDPHandler : public UserDatagramProtocolHandler {
public:
    void HandleUserDatagramProtocolMessage(UserDatagramProtocolSocket* socket,
                                           common::uint8_t* data,
                                           common::uint16_t size) {
        char* foo = " ";
        for(int i = 0; i < size; i++) {
            foo[0] = data[i];
            printf(foo);
        }
    }
};

class PrintfTCPHandler : public TransmissionControlProtocolHandler {
public:
    bool HandleTransmissionControlProtocolMessage(TransmissionControlProtocolSocket* socket,
                                                  common::uint8_t* data,
                                                  common::uint16_t size) {
        printf("\nData received ");
        char* foo = " ";
        for(int i = 0; i < size; i++) {
            foo[0] = data[i];
            printf(foo);
        }
        
        if(size > 9
            && data[0] == 'G'
            && data[1] == 'E'
            && data[2] == 'T'
            && data[3] == ' '
            && data[4] == '/'
            && data[5] == ' '
            && data[6] == 'H'
            && data[7] == 'T'
            && data[8] == 'T'
            && data[9] == 'P'
        ) {
            socket->Send((uint8_t*)"HTTP/1.1 200 OK\r\nServer: MyOS\r\nContent-Type: text/html\r\n\r\n<html><head><title>My Operating System</title></head><body><b>My Operating System</b> http://www.AlgorithMan.de</body></html>\r\n",184);
            socket->Disconnect();
        }

        return true;
    }
};


void sysprintf(char *str) {
    asm("int $0x80" : : "a" (4), "b" (str));
}

// Task for testing
void taskA() {
    while (true)
        sysprintf("A");
}

void taskB() {
    while (true)
        sysprintf("B");
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
    printf("IregonOS --- https://github.com/iregon/IregonOS\n");

    GlobalDescriptorTable gdt;

    uint32_t *memupper = (uint32_t * )(((size_t) multiboot_structure) + 8);
    size_t heap = 10 * 1024 * 1024;
    MemoryManager memoryManager(heap, (*memupper) * 1024 - heap - 10 * 1024);

    // BEGIN Dynamic memory management
    printf("heap: 0x");
    printfHex((heap >> 24) & 0xFF);
    printfHex((heap >> 16) & 0xFF);
    printfHex((heap >> 8) & 0xFF);
    printfHex((heap) & 0xFF);

    void *allocated = memoryManager.malloc(1024);
    printf("\nallocated: 0x");
    printfHex(((size_t) allocated >> 24) & 0xFF);
    printfHex(((size_t) allocated >> 16) & 0xFF);
    printfHex(((size_t) allocated >> 8) & 0xFF);
    printfHex(((size_t) allocated) & 0xFF);
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
    SyscallHandler syscalls(&interrupts, 0x80);

    printf("Initializing Hardware, Stage 1\n");

#ifdef GRAPHICSMODE
    Desktop desktop(320,200, 0x00,0x00,0xA8);
#endif

    DriverManager drvManager;

    // BEGIN Keyboard interrupt handler and driver
#ifdef GRAPHICSMODE
    KeyboardDriver keyboard(&interrupts, &desktop);
#else
    PrintfKeyboardEventHandler kbhandler;
    KeyboardDriver keyboard(&interrupts, &kbhandler);
#endif

    drvManager.AddDriver(&keyboard);
    // END

    // BEGIN Mouse interrupt handler and driver
#ifdef GRAPHICSMODE
    MouseDriver mouse(&interrupts, &desktop);
#else
    MouseToConsole mousehandler;
    MouseDriver mouse(&interrupts, &mousehandler);
#endif

    drvManager.AddDriver(&mouse);
    // END

    // BEGIN PCI
    PeripheralComponentInterconnectController PCIController;
    PCIController.SelectDrivers(&drvManager, &interrupts);
    // END

#ifdef GRAPHICSMODE
    VideoGraphicsArray vga;
#endif

    printf("Initializing Hardware, Stage 2\n");
    drvManager.ActivateAll();

    printf("Initializing Hardware, Stage 3\n");

#ifdef GRAPHICSMODE
    vga.SetMode(320, 200, 8);

    Window win1(&desktop, 10, 10, 20, 20, 0xA8, 0x00, 0x00);
    desktop.AddChild(&win1);

    Window win2(&desktop, 40, 15, 30, 30, 0x00, 0xA8, 0x00);
    desktop.AddChild(&win2);
#endif

    printf("S-ATA primary master: ");
    AdvancedTechnologyAttachment ata0m(true, 0x1F0);
    ata0m.Identify();
    /*
    ata0m.Write28(0, (uint8_t *) "https://github.com/iregon/IregonOS", 34);
    ata0m.Flush();
    ata0m.Read28(0, 34);
    */

    /*
    printf("\nS-ATA primary slave: ");
    AdvancedTechnologyAttachment ata0s(false, 0x1F0);

    printf("\nS-ATA secondary master: ");
    AdvancedTechnologyAttachment ata1m(true, 0x170);
    ata1m.Identify();

    printf("\nS-ATA secondary slave: ");
    AdvancedTechnologyAttachment ata1s(false, 0x170);
    ata1s.Identify();
    */

    // third: 0x1E8
    // fourth: 0x168

    // BEGIN Networking

    amd_am79c973 *eth0 = (amd_am79c973 * )(drvManager.drivers[2]);

    // IP Address
    uint8_t ip1 = 10, ip2 = 0, ip3 = 2, ip4 = 15;
    uint32_t ip_be = ((uint32_t) ip4 << 24)
                     | ((uint32_t) ip3 << 16)
                     | ((uint32_t) ip2 << 8)
                     | (uint32_t) ip1;
    eth0->SetIPAddress(ip_be);
    EtherFrameProvider etherframe(eth0);
    AddressResolutionProtocol arp(&etherframe);

    // IP Address of the default gateway
    uint8_t gip1 = 10, gip2 = 0, gip3 = 2, gip4 = 2;
    uint32_t gip_be = ((uint32_t) gip4 << 24)
                      | ((uint32_t) gip3 << 16)
                      | ((uint32_t) gip2 << 8)
                      | (uint32_t) gip1;

    uint8_t subnet1 = 255, subnet2 = 255, subnet3 = 255, subnet4 = 0;
    uint32_t subnet_be = ((uint32_t) subnet4 << 24)
                         | ((uint32_t) subnet3 << 16)
                         | ((uint32_t) subnet2 << 8)
                         | (uint32_t) subnet1;

    InternetProtocolProvider ipv4(&etherframe, &arp, gip_be, subnet_be);
    InternetControlMessageProtocol icmp(&ipv4);
    UserDatagramProtocolProvider udp(&ipv4);
    TransmissionControlProtocolProvider tcp(&ipv4);

    //etherframe.Send(0xFFFFFFFFFFFF, 0x0608, (uint8_t*)"FOO", 3);
    //eth0->Send((uint8_t*)"Hello Network", 13);
    

    interrupts.Activate();

    arp.BroadcastMACAddress(gip_be);
    
    tcp.Connect(gip_be, 1234);
    PrintfTCPHandler tcphandler;
    TransmissionControlProtocolSocket* tcpsocket = tcp.Listen(1234);
    tcp.Bind(tcpsocket, &tcphandler);
    //tcpsocket->Send((uint8_t*)"Hello TCP!", 10);
    
    icmp.RequestEchoReply(gip_be);
    
    //PrintfUDPHandler udphandler;
    //UserDatagramProtocolSocket* udpsocket = udp.Connect(gip_be, 1234);
    //udp.Bind(udpsocket, &udphandler);
    //udpsocket->Send((uint8_t*)"Hello UDP!", 10);

    //UserDatagramProtocolSocket* udpsocket = udp.Listen(1234);
    //udp.Bind(udpsocket, &udphandler);
    
    // END

    while (1) {
#ifdef GRAPHICSMODE
        desktop.Draw(&vga);
#endif
    }
}
