#include <stddef.h>
#include <stdint.h>

// Port definitions for hypercalls
#define PORT_PRINT_CHAR 0xE9    // Existing port for character printing
#define PORT_PRINT_VAL  0xEA    // New port for printing 32-bit values
#define PORT_GET_EXITS  0xEB    // New port for getting exit count

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

void
__attribute__((noreturn))
__attribute__((section(".start")))
_start(void) {
	const char *p;

	// Print hello world as before
	for (p = "Hello, world!\n"; *p; ++p)
		outb(PORT_PRINT_CHAR, *p);

	// Test the new printVal function
	printVal(42);
	printVal(0xDEADBEEF);

	// Get and print the number of exits
	uint32_t numExits = getNumExits();
	printVal(numExits);

	*(volatile long *)0x400 = 42;

	for (;;)
		asm("hlt" : /* empty */ : "a" (42) : "memory");
}
