#pragma once

class Core
{
public:

	bool DrawFlag; //True= Render GFX memory, return to false once drawing is done

	unsigned char key[16]; // Keypad input is managed by 16 individual buttons, which are represented by a singular byte, of on or off.

	Core();

	void Initialise(); //Intialise memory and key systems of the Chip8

	bool LoadGame(const char* Program); //Take a file and load directly into RAM

	void Emulate(); //Emulation cycle


	unsigned char* GetGfx();

private:
	
	unsigned short Opcode;//Stores current opcode to be executed by the CPU, this is derived from the program counter and what is currently on the stack. 2 bytes are enough to store an opcode

	unsigned char Memory[4096]; //The Chip-8 standard had 4 kilobytes of RAM, this can be represented by 4096 bytes, which is 4096 chars.

	unsigned char V[16]; // Chip 8 has 15 8 bit general purpose registers, whilst the 16th acts as a carry flag.

	unsigned short I; //The index and program counter are used to access memory and what is what currently on the stack.
	unsigned short PC; 

	unsigned char gfx[64 * 32]; //Pixels in chip 8 can only be Black(off) or White (on). This state can be represented as 0 or 1.


	unsigned char delay_timer; //These timers count to 0 when they are set above 0, the buzzer sounds whenever the sound timer hits 0
	unsigned char sound_timer;

	unsigned short stack[16]; //Although the specification doesn't have a stack, one needs to be implemented and a pointer to access this as well.
	unsigned short sp; 




};