#include "keyboard.h"

KeyboardDriver::KeyboardDriver(InterruptManager* manager)
: InterruptHandler(manager, 0x21),
  dataport(0x60),
  commandport(0x64) {
    while(commandport.Read() & 0x1)
        dataport.Read();
    
    commandport.Write(0xae); // activate interrupts
    commandport.Write(0x20); // command 0x20 = read controller command byte
    uint8_t status = (dataport.Read() | 1) & ~0x10;
    commandport.Write(0x60); // command 0x60 = set controller command byte
    dataport.Write(status);
    dataport.Write(0xf4);
}

KeyboardDriver::~KeyboardDriver() {
}

void printf(char*);

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp) {
    uint8_t key = dataport.Read();
    
    static bool is_shift_pressed = false;
    static bool is_capslock_active = false;
    
    switch(key) {
        case 0x02: if (is_shift_pressed || is_capslock_active) printf("!"); else printf("1"); break;
        case 0x03: if (is_shift_pressed || is_capslock_active) printf("\""); else printf("2"); break;
        case 0x04: if (is_shift_pressed || is_capslock_active) printf("£"); else printf("3"); break;
        case 0x05: if (is_shift_pressed || is_capslock_active) printf("$"); else printf("4"); break;
        case 0x06: if (is_shift_pressed || is_capslock_active) printf("%"); else printf("5"); break;
        case 0x07: if (is_shift_pressed || is_capslock_active) printf("&"); else printf("6"); break;
        case 0x08: if (is_shift_pressed || is_capslock_active) printf("/"); else printf("7"); break;
        case 0x09: if (is_shift_pressed || is_capslock_active) printf("("); else printf("8"); break;
        case 0x0A: if (is_shift_pressed || is_capslock_active) printf(")"); else printf("9"); break;
        case 0x0B: if (is_shift_pressed || is_capslock_active) printf("="); else printf("0"); break;

        case 0x10: if (is_shift_pressed || is_capslock_active) printf("Q"); else printf("q"); break;
        case 0x11: if (is_shift_pressed || is_capslock_active) printf("W"); else printf("w"); break;
        case 0x12: if (is_shift_pressed || is_capslock_active) printf("E"); else printf("e"); break;
        case 0x13: if (is_shift_pressed || is_capslock_active) printf("R"); else printf("r"); break;
        case 0x14: if (is_shift_pressed || is_capslock_active) printf("T"); else printf("t"); break;
        case 0x15: if (is_shift_pressed || is_capslock_active) printf("Y"); else printf("y"); break;
        case 0x16: if (is_shift_pressed || is_capslock_active) printf("U"); else printf("u"); break;
        case 0x17: if (is_shift_pressed || is_capslock_active) printf("I"); else printf("i"); break;
        case 0x18: if (is_shift_pressed || is_capslock_active) printf("O"); else printf("o"); break;
        case 0x19: if (is_shift_pressed || is_capslock_active) printf("P"); else printf("p"); break;

        case 0x1E: if (is_shift_pressed || is_capslock_active) printf("A"); else printf("a"); break;
        case 0x1F: if (is_shift_pressed || is_capslock_active) printf("S"); else printf("s"); break;
        case 0x20: if (is_shift_pressed || is_capslock_active) printf("D"); else printf("d"); break;
        case 0x21: if (is_shift_pressed || is_capslock_active) printf("F"); else printf("f"); break;
        case 0x22: if (is_shift_pressed || is_capslock_active) printf("G"); else printf("g"); break;
        case 0x23: if (is_shift_pressed || is_capslock_active) printf("H"); else printf("h"); break;
        case 0x24: if (is_shift_pressed || is_capslock_active) printf("J"); else printf("j"); break;
        case 0x25: if (is_shift_pressed || is_capslock_active) printf("K"); else printf("k"); break;
        case 0x26: if (is_shift_pressed || is_capslock_active) printf("L"); else printf("l"); break;

        case 0x2C: if (is_shift_pressed || is_capslock_active) printf("Z"); else printf("z"); break;
        case 0x2D: if (is_shift_pressed || is_capslock_active) printf("X"); else printf("x"); break;
        case 0x2E: if (is_shift_pressed || is_capslock_active) printf("C"); else printf("c"); break;
        case 0x2F: if (is_shift_pressed || is_capslock_active) printf("V"); else printf("v"); break;
        case 0x30: if (is_shift_pressed || is_capslock_active) printf("B"); else printf("b"); break;
        case 0x31: if (is_shift_pressed || is_capslock_active) printf("N"); else printf("n"); break;
        case 0x32: if (is_shift_pressed || is_capslock_active) printf("M"); else printf("m"); break;
        case 0x33: if (is_shift_pressed || is_capslock_active) printf(";"); else printf(","); break;
        case 0x34: if (is_shift_pressed || is_capslock_active) printf(":"); else printf("."); break;
        case 0x35: if (is_shift_pressed || is_capslock_active) printf("_"); else printf("-"); break;
        
        // keys to ignore
        case 0x1D: // CTRL sx
        case 0x38: // ALT
        case 0x48: case 0x4B: case 0x50: case 0x4D: // arrows (top, right, bottom, left)
        case 0x5b: // Windows
            break;
            
        case 0x2A: case 0x36: is_shift_pressed = true; break; // SHIFT pressed
        case 0xAA: case 0xB6: is_shift_pressed = false; break; // SHIFT relased
        
        // Capslock
        case 0x3A: is_capslock_active = !is_capslock_active; break;
        
        // TAP
        case 0x0F: printf("\t"); break;
        
        case 0x1C: printf("\n"); break;
        case 0x39: printf(" "); break;

        default:
        {
            if(key < 0x80) {
                char* foo = "KEYBOARD 0x00 ";
                char* hex = "0123456789ABCDEF";
                foo[11] = hex[(key >> 4) & 0xF];
                foo[12] = hex[key & 0xF];
                printf(foo);
                break;
            }
        }
    }
    return esp;
}
