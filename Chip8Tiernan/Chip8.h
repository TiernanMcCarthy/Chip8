#pragma once



class Chip8
{
public:
	Chip8();
	~Chip8();

	void initialize();
	void debugRender();
	void emulateCycle();

	bool drawflag;
	bool LoadProgram(const char *Program);

private:

	//All variables storing data should be unsigned, as they are just memory that the emulator manipulates
//The actual program would decide if these were signed or not, you are just reserving space

	unsigned short opcode; //Store the Opcode operation here, A short is two btyes

	unsigned char memory[4096]; //4 Kilobytes of memory stored by the 1 byte char

	//15 8 Bit general purpose registers named V0 - VE the 16th is for a carry flag

	unsigned char V[16]; //Store registers here

	unsigned short I; //Index Register

	unsigned short pc; //Program Counter that specifies where in the stack the instruction is

	/*
	0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
	0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
	0x200-0xFFF - Program ROM and work RAM
	*/


	//Drawing is done in XOR mode and if a pixel is turned off as a reslt of drawing the register is set for collision.
	unsigned char gfx[64 * 32]; //64x32 "Pixel" screen. Black or White are the only options, each value should be 0 or 1


	//There are two timer registers that count at 60hz and count down to 0 when above 0

	unsigned char delay_timer;
	unsigned char sound_timer;

	unsigned short stack[16]; //16 long stack of 2 byte opcodes 2 Byte Shorts
	unsigned short sp; //Stack pointer, can collect this opcode.

	unsigned char key[16]; //HEX based keypad 0x0-0xF. Stores input
};

