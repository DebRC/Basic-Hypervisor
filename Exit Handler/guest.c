#include <stddef.h>
#include <stdint.h>

// Flag for HC_numExitsByType
int exitByTypeBufferFlag=0;
// Buffer for HC_numExitsByType
static char firstBuffer[50];
static char secondBuffer[50];


// Out Binary
static void outb(uint16_t port, uint8_t value)
{
	asm("outb %0,%1" : /* empty */ : "a"(value), "Nd"(port) : "memory");
}

// Out Generic
static void outl(uint16_t port, uint32_t value)
{
	asm("out %0,%1" : /* empty */ : "a"(value), "Nd"(port) : "memory");
}

// In Generic
static inline uint32_t in(uint16_t port) {
	uint32_t ret;
	asm("in %1, %0" : "=a"(ret) : "Nd"(port) : "memory" );
	return ret;
}

void HC_print8bit(uint8_t val)
{
	outb(0xE9, val);
}

void HC_print32bit(uint32_t val)
{
	outl(0xE8, val);
}

uint32_t HC_numExits()
{
	uint32_t exits = in(0xE7);
	return exits;
}

void HC_printStr(char *str)
{
    outl(0xE6, (uint32_t)(uintptr_t)str);
}

char *HC_numExitsByType()
{
    char *exits = (char *)(uintptr_t)in(0xE5);

	// Select buffer according to call
    char *buffer = (exitByTypeBufferFlag == 0) ? firstBuffer : secondBuffer;

	// Change global flag
	exitByTypeBufferFlag = (exitByTypeBufferFlag == 0) ? 1 : 0;

	// String copy from exits to buffer
	int i=0;
    for (i = 0; exits[i] != '\0'; i++) {
        buffer[i] = exits[i];
    }
	buffer[i]='\0';
    return buffer;
}

uint32_t HC_gvaToHva(uint32_t gva)
{
	outl(0xE4, (uint32_t)(uintptr_t)&gva);
	return gva;
}

void
__attribute__((noreturn))
__attribute__((section(".start")))
_start(void) {
	const char *p;

	for (p = "Hello 695!\n"; *p; ++p)
		HC_print8bit(*p);


	/*----------Don't modify this section. We will use grading script---------*/
	/*---Your submission will fail the testcases if you modify this section---*/
	HC_print32bit(2048);
	HC_print32bit(4294967295);

	uint32_t num_exits_a, num_exits_b;
	num_exits_a = HC_numExits();

	char *str = "CS695 Assignment 2\n";
	HC_printStr(str);

	num_exits_b = HC_numExits();

	HC_print32bit(num_exits_a);
	HC_print32bit(num_exits_b);

	char *firststr = HC_numExitsByType();
	uint32_t hva;
	hva = HC_gvaToHva(1024);
	HC_print32bit(hva);
	hva = HC_gvaToHva(4294967295);
	HC_print32bit(hva);
	char *secondstr = HC_numExitsByType();

	HC_printStr(firststr);
	HC_printStr(secondstr);
	/*------------------------------------------------------------------------*/

	*(long *) 0x400 = 42;

	for (;;)
		asm("hlt" : /* empty */ : "a" (42) : "memory");
}