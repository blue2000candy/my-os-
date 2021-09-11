#pragma once
#include "../IO.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

struct interrupt_frame;
void PIC_EndMaster(){
    outb(PIC1_COMMAND, PIC_EOI);
}

void PIC_EndSlave(){
    outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}
__attribute__((interrupt)) void KeyboardInt_Handler(struct interrupt_frame* frame){
    
    uint8_t scancode = inb(0x60);
    
    Print(g_framebuffer,g_font,0xffffffff,to_string(scancode),400,100+_top);
    _top += 16;
    PIC_EndMaster();
  
}  unsigned int lines = 0 ;
__attribute__((interrupt)) void mouseInt_Handler(struct interrupt_frame* frame){
    
    uint8_t mouse_date = inb(0x60);
    
    if(_left +300 > g_framebuffer->Width){_left = 0;lines++;}
    Print(g_framebuffer,g_font,0xffffffff,"h",200+_left,100+ lines*16);
    _left += 16;
    PIC_EndSlave();
    //
}  

void RemapPIC(){
    uint8_t a1, a2; 

    a1 = inb(PIC1_DATA);
    io_wait();
    a2 = inb(PIC2_DATA);
    io_wait();

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x28);
    io_wait();

    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    outb(PIC1_DATA, a1);
    io_wait();
    outb(PIC2_DATA, a2);

}