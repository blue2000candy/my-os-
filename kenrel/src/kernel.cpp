#include <stdint.h>
#include <stddef.h>
#include "library/graphics.h"
#include "library/memory/EfiMemory.h"
#include "library/memory/memory.h"
#include "library/memory/PageFrameAllocator.h"
#include "library/gdt/gdt.h"
#include "library/interrupts/IDT.h"
#include "library/interrupts/interrupts.h"
#include "library/IO.h"
#include "library/user_input/mouse.h"
PageFrameAllocator main_PFA;

IDTR idtr;
void PrepareInterrupts(){
    idtr.Limit = 0x0FFF;
    idtr.Offset = (uint64_t)main_PFA.RequestPage();

    IDTDescEntry* int_Keyboard = (IDTDescEntry*)(idtr.Offset + 0x21 * sizeof(IDTDescEntry));
    int_Keyboard->SetOffset((uint64_t)KeyboardInt_Handler);
    int_Keyboard->type_attr = IDT_TA_InterruptGate;
    int_Keyboard->selector = 0x08;
    //*
    IDTDescEntry* int_mouse = (IDTDescEntry*)(idtr.Offset + 0x2C * sizeof(IDTDescEntry));
    int_mouse->SetOffset((uint64_t)mouseInt_Handler);
    int_mouse->type_attr = IDT_TA_InterruptGate;
    int_mouse->selector = 0x08;//*/

    asm ("lidt %0" : : "m" (idtr));

    RemapPIC();
    //*
    InitPS2Mouse();
    
    
    outb(PIC1_DATA, 0b11111001);
    outb(PIC2_DATA, 0b11101111);//*/

    /*
    outb(PIC1_DATA, 0b11111101);
    outb(PIC2_DATA, 0b11111111);//*/

    asm ("sti");

}

extern "C" int _start(Framebuffer* framebuffer,PSF1_FONT* font,EFI_MEMORY_DESCRIPTOR* MMap,uint64_t mMap_size, uint64_t mMap_DescSize){
    g_framebuffer = framebuffer;
    g_font = font;
	draw_rectangle(framebuffer,1,1,framebuffer->Width-2,framebuffer->Height-2,0xff004444);
	GDTDescriptor gdtDescriptor;
    
    gdtDescriptor.Size = sizeof(GDT) -1;
    gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
    
    LoadGDT(&gdtDescriptor);

    uint64_t mMapEntries = mMap_size / mMap_DescSize;
    unsigned int y = 0 ;
    /*for (int i = 0; i < mMapEntries; i++){
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)MMap + (i * mMap_DescSize));
        Print(framebuffer,font,0xffffffff,EFI_MEMORY_TYPE_STRINGS[desc->type],0,y);
        y+=16;
        
    }*/

    main_PFA.ReadEFIMemoryMap(MMap,mMap_size,mMap_DescSize);
    Print(framebuffer,font,0xffffffff,to_string(GetMemorySize(MMap,mMapEntries,mMap_DescSize)),400,0);
    Print(framebuffer,font,0xffffffff,to_string(main_PFA.GetFreeRAM()/1024),400,16);
    Print(framebuffer,font,0xffffffff,to_string(main_PFA.GetReservedRAM()/1024),400,16+16);
    Print(framebuffer,font,0xffffffff,to_string(main_PFA.GetUsedRAM()/1024),400,16+16+16);
    
    PrepareInterrupts();
    
    while(true);
	return 6;
}