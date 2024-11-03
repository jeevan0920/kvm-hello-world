#include <stddef.h>
#include <stdint.h>

// Port definitions for hypercalls
#define PORT_PRINT_CHAR 0xE9    // Existing port for character printing
#define PORT_PRINT_VAL  0xEA    // New port for printing 32-bit values
#define PORT_GET_EXITS  0xEB    // New port for getting exit count
#define PORT_DISPLAY_STR 0xEC   // New port for displaying string

static void outb(uint16_t port, uint32_t value) {
	asm("out %0,%1" : /* empty */ : "a" (value), "Nd" (port) : "memory");
}

static uint32_t inb(uint16_t port) {
	uint32_t ret;
	asm("in %1, %0" : "=a"(ret) : "Nd"(port) : "memory" );
	return ret;
}

static void printVal(uint32_t val) {
	outb(PORT_PRINT_VAL, val);
}

static uint32_t getNumExits(void) {
	return inb(PORT_GET_EXITS);
}

static void display(const char *str) {
	// Pass the address of the string buffer to hypervisor
	outb(PORT_DISPLAY_STR, (uintptr_t)str);
}

void
__attribute__((noreturn))
__attribute__((section(".start")))
_start(void) {
	const char *p = "Hello, world!\n";
	
	// Get initial exit count
	uint32_t exits_before = getNumExits();
	
	// Display string with single exit
	display(p);
	
	// Get exit count after display
	uint32_t exits_after = getNumExits();
	
	// Print exit counts to verify single exit
	printVal(exits_before);
	printVal(exits_after);
	
	*(volatile long *)0x400 = 42;

	for (;;)
		asm("hlt" : /* empty */ : "a" (42) : "memory");
}
