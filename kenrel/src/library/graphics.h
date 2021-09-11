

typedef struct {
	void* BaseAddress;
	unsigned long long BufferSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelsPerScanLine;
} Framebuffer;

void draw_rectangle(Framebuffer* drawBuffer,unsigned int top , unsigned int left ,unsigned int Width , unsigned int Height,unsigned int color){
	unsigned int bbP = 4;
	for (unsigned int y = top; y < top + Height; y++){
		for(unsigned int x = left * bbP;x < (Width + left) * bbP; x+=bbP){
			*(unsigned int*)(x + (y* drawBuffer->PixelsPerScanLine * bbP)+ drawBuffer->BaseAddress) = color;
		}
	}
}

typedef struct {
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
} PSF1_HEADER;

typedef struct {
	PSF1_HEADER* psf1_Header;
	void* glyphBuffer;
} PSF1_FONT;

Framebuffer*  g_framebuffer;
PSF1_FONT* g_font;
unsigned int _top;
unsigned int _left;

void putChar(Framebuffer* framebuffer, PSF1_FONT* psf1_font, unsigned int colour, short chr, unsigned int xOff, unsigned int yOff){
    unsigned int* pixPtr = (unsigned int*)framebuffer->BaseAddress;
    char* fontPtr = (char*)psf1_font->glyphBuffer + (chr * psf1_font->psf1_Header->charsize);
    for (unsigned long y = yOff; y < yOff + 16; y++){
        for (unsigned long x = xOff; x < xOff+8; x++){
            if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0){
                    *(unsigned int*)(pixPtr + x + (y * framebuffer->PixelsPerScanLine)) = colour;
                }

        }
        fontPtr++;
    }
}

void Print(Framebuffer* framebuffer, PSF1_FONT* psf1_font, unsigned int colour,const  char* str,unsigned int X ,unsigned int Y ){
    
	unsigned int m = 0;
    while(*str != 0){
        putChar(framebuffer, psf1_font, colour, (short)*str, X, Y);
        X+=8;
        str++;
    }
}

char uintTo_StringOutput[128];
const char* to_string(uint64_t value){
    uint8_t size;
    uint64_t sizeTest = value;
    while (sizeTest / 10 > 0){
        sizeTest /= 10;
        size++;
    }

    uint8_t index = 0;
    while(value / 10 > 0){
        uint8_t remainder = value % 10;
        value /= 10;
        uintTo_StringOutput[size - index] = remainder + '0';
        index++;
    }
    uint8_t remainder = value % 10;
    uintTo_StringOutput[size - index] = remainder + '0';
    uintTo_StringOutput[size + 1] = 0; 
    return uintTo_StringOutput;
}