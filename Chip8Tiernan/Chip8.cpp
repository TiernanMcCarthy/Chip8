#include "Chip8.h"
#include <stdio.h>
#include <stdlib.h> //Used for rand function. Not a perfect randomisation, but I doubt the Chip8 equivalent was either
#include <time.h>
Chip8::Chip8()
{
}


Chip8::~Chip8()
{
}
unsigned char chip8_fontset[80] =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};



void Chip8::initialize()
{
	srand(time(NULL)); //Create a seed for the random function
	
	pc = 0x200;  // Program counter starts at 0x200
	opcode = 0;      // Reset current opcode	
	I = 0;      // Reset index register
	sp = 0;      // Reset stack pointer

	// Clear display
	for (int i = 0; i < 2048; i++)
	{
		gfx[i] = 0;
	}
	// Clear stack
	for (int i = 0; i < 16; i++)
	{
		stack[i] = 0;
	}
	// Clear registers V0-VF
	for (int i = 0; i < 16; i++)
	{
		key[i]=V[i] = 0;
	}
	// Clear memory
	for (int i = 0; i < 4096; i++)
	{
		memory[i] = 0;
	}
	// Load fontset into the first 80 bytes of memory
	for (int i = 0; i < 80; ++i)
		memory[i] = chip8_fontset[i]; //Define font set soon

	// Reset timers
	delay_timer = 0;
	sound_timer = 0;

	drawflag = true;

	//srand?
}


bool Chip8::LoadProgram(const char* Program)
{
	printf("Loading Program\n");
	printf(Program ,"\n");
	//Access the File in this set directory
	FILE *ROM;
	fopen_s(&ROM, Program, "rb"); //Read Binary fopen is deprecated
	//FILE *ROM = fopen(Program, "rb"); //r is read and b just means binary file

	if (ROM==NULL)
	{
		printf("File Error, possible that this was not a ROM \n", stderr);
	}

	//Check File Size
	fseek(ROM, 0, SEEK_END);
	long Size = ftell(ROM); //For some reason this value is stored elsewhere

	rewind(ROM);

	printf("FileSize: %d\n", (int)Size); //Report the size of the file

	//Allocate memory to contain the whole file
	char *buffer = (char*)malloc(sizeof(char)* Size); //Allocate size with chars which are one byte

	if (buffer == NULL)
	{
		fputs("Memory Error", stderr);
		return false;
	}

	//copy the file into the buffer

	size_t result = fread(buffer, 1, Size, ROM);
	if (result!=Size)
	{
		printf("Reading Error", stderr);
	}

	//Copy buffer into Chip8 memory

	//512 Bytes are reserved for the rom
	if ((4096 - 512) > Size)
	{
		for (int i = 0; i < Size; i++)
			memory[i + 512] = buffer[i];
	}
	else
		printf("Error: This is not a valid Chip8 File. This exceeds the size limitations\n");

	//close file, free the memory buffer
	fclose(ROM);
	free(buffer);
	
	printf("Loading Successful\n");


	return true;
}




//Missing 0NNN but its not necessary for most ROMS and loads the RCA 1802 program, whatever that is
 
int AddressLength;
int temp;
void Chip8::emulateCycle()
{
	// Fetch opcode
	opcode = memory[pc] << 8 | memory[pc + 1];

	// Decode opcode
	switch (opcode & 0xF000)
	{
		// Some opcodes //

	case 0xA000: // ANNN: Sets I to the address NNN
	  // Execute opcode
		I = opcode & 0x0FFF;
		pc += 2;
		break;


	case 0xB000: //BNNN: Jumps to address NNN plus V0
		//I = opcode & 0x0FFF;
		pc = (opcode & 0x0FFF) + V[0];
		break;

	case 0xC000: //CNNN //Rand() %100 is 0-99 and rand() %100+1 is 1-100 The example is wrong with %255?
		V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00FF)] & (rand() % 256);
		pc += 2;
		break;
		//case 0xD000:
			//V[]


	case 0x1000://1NNN Goto NNN
		pc = opcode & 0x0FFF;
		break;
	case 0x2000: //2NNN Calls subroutine at 0xNNN
		stack[sp] = pc;
		++sp;
		pc = opcode & 0x0FFF;
		break;
	case 0x3000: //3NNN Skips the next instruction if Vx==NN
		if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
		{
			pc += 4;
		}
		else
			pc += 2;
		break;
	case 0x4000: //4NNN skips the next instruction if Vx!=NN
		if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
		{
			pc += 4;
		}
		else
			pc += 2;
		break;

	case 0x5000: //5XY0 skips the next instruction if Vx= Vy
		if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
		{
			pc += 4;
		}
		else
			pc += 2;
		break;
	case 0x6000: //6XNN Sets Vx to NN
		V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
		pc += 2;
		break;

	case 0x7000: //Adds NN to VX
		V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
		pc += 2;
		break;



	case 0x8000:
		switch (opcode & 0x000F)
		{
		case 0x0000: //8XY0 //Vx=Vy 
			V[(opcode & 0x00F0) >> 8] = V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;

		case 0x0001: //8XY1 //Vx or equals Vy 
			V[(opcode & 0x00F0) >> 8] |= V[(opcode & 0x00F0)];
			pc += 2;
			break;

		case 0x0002: //8XY2 // If Vx == Vy 
			if (V[(opcode & 0x00F0) >> 8] == V[(opcode & 0x00F0)])
			{
				pc += 4;
				break;
			}
			pc += 2;
			break;

		case 0x0003: //8XY3 Sets VX to xor VY
			V[(opcode & 0x00F0) >> 8] ^= V[(opcode & 0x00F0)];
			pc += 2;
			break;
		case 0x0004://8XY4 //Vx += Vy 
			if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
				V[0xF] = 1; //carry
			else
				V[0xF] = 0;
			V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		case 0x0005: // 8XY5 Vx- Vy
			if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
				V[0xF] = 0; //Set the carry flag
			else
				V[0xF] = 1;
			V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
			pc + 2;
			break; //
		case 0x0006: //8XY6 Shifts VX by one and stores the original value in VF
			V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
			V[(opcode & 0x0F00) >> 8] >>= 1;
			pc += 2;
			break;
		case 0x0007: //8XY7 Set Vx to Vy-Vx and set carry flag
			if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
				V[0xF] = 1;
			else
				V[0xF] = 0;
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;
		case 0x000E: //8XYE //Stores the Most significant bit of VX in VF and then shift VX to the left by 1
			V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
			V[(opcode & 0x0F00) >> 8] <<= 1;
			pc += 2;
			break;
		default:
			printf("Unknown opcode [0x8000]: 0x%X\n", opcode);
		}
		break;
	case 0x9000: //9XY0    Skips the next instruction if VX!= VY 
		if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
		{
			pc += 4;
			break;
		}
		pc += 2;
		break;


		//case 0x0033: 
		//	memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
		//	memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
		//	memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
		//	pc += 2;
		//	break;

	case 0xD000: //Draw DXYN
	{
		unsigned short x = V[(opcode & 0x0F00) >> 8];
		unsigned short y = V[(opcode & 0x00F0) >> 4];
		unsigned short height = opcode & 0x000F;
		unsigned short pixel;

		V[0xF] = 0;
		for (int yline = 0; yline < height; yline++)
		{
			pixel = memory[I + yline];
			for (int xline = 0; xline < 8; xline++)
			{
				if ((pixel & (0x80 >> xline)) != 0)
				{
					if (gfx[(x + xline + ((y + yline) * 64))] == 1)
						V[0xF] = 1;
					gfx[x + xline + ((y + yline) * 64)] ^= 1;
				}
			}
		}

		drawflag = true;
		pc += 2;
	}

	case 0xF000: //FX Opcodes
		switch (opcode & 0x00FF)
		{
		case 0x0007: //FX07 Sets VX to the Delay Timer
			V[(opcode & 0x0F00) >> 8] = delay_timer;
			pc += 2;
			break;
		case 0x000A: //FX0A Sets VX to the expected key press, await it
			bool Key;

			for (short i = 0; i < 16; i++)
			{
				if (key[i] != 0)
				{
					V[(opcode && 0x0F00) >> 8] = i; //Set VX to the current key ID represented as i
					Key = true;
				}
			}

			if (Key != true) //Keep waiting until you get this key press
			{
				return;
			}

			pc += 2;
			break;

		case 0x0015: //FX15 //Set the delay timer to VX
			delay_timer = V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;

		case 0x0018: //FX18 Sets the sound timer to VX
			sound_timer = V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;
		case 0x001E: //FX1E Adds VX to index
			if (I + V[(opcode & 0x0F00) >> 8] > 255) //Overflow Case
			{
				V[0xF] = 1;
			}
			else
				V[0xF] = 0;
			I += V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;

		case 0x0029: // FX29: Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
			I = V[(opcode & 0x0F00) >> 8] * 0x5;
			pc += 2;
			break;
		case 0x0033: //FX33 Split the individual components into sections of memory starting at I. Chip 8 is big endian
			temp = V[(opcode & 0x0F00) >> 8];
			memory[I] = temp / 100;
			memory[I + 1] = (temp / 10) % 10;
			memory[I + 2] = (temp % 100) % 10;
			pc += 2;
			break;

		case 0x0055: //FX55
			AddressLength = (opcode & 0x0F00) >> 8;
			for (short i = 0; i < AddressLength; i++)
			{
				memory[I + i] = V[i]; //Set the memory to be the values contained within these registers
			}

			I += AddressLength + 1; //I=I+RegisterXCount+1;
			pc += 2;
			break;

		case 0x0065: //FX65
			AddressLength = (opcode & 0x0F00) >> 8;
			for (short i = 0; i < AddressLength; i++)
			{
				V[i] = memory[I + i];
			}
			I += AddressLength + 1; //I=I+RegisterXCount+1;
			pc += 2;
			break;

		}


		//default:
		//printf("Unknown Opcode [0x0F000]: 0x%X\n", opcode);
		break;
	case 0xE000: //Input
		switch (opcode & 0x00FF)
		{
			// EX9E: Skips the next instruction 
			// if the key stored in VX is pressed
		case 0x009E:
			if (key[V[(opcode & 0x0F00) >> 8]] != 0)
				pc += 4;
			else
				pc += 2;
			break;
		case 0x00A1: //EXA1 Skips the next instruction if the key in VX isn't pressed
			if (key[V[(opcode & 0x0F00) >> 8]] == 0)
			{
				pc += 4;
				break;
			}
			pc += 2;
			break;


			break;
		default:
			printf("Unknown opcode: 0x%X\n", opcode);
		}

		switch (opcode & 0xF000)
		{
		case 0x0000:
			switch (opcode & 0x000F)
			{
			case 0x0000: // 0x00E0: Clears the screen        
			  // Execute opcode
				break;

			case 0x000E: // 0x00EE: Returns from subroutine          
			  // Execute opcode
				break;

			default:
				printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
			}
			break;

			// more opcodes //
		}
		// Update timers
		if (delay_timer > 0)
			--delay_timer;

		if (sound_timer > 0)
		{
			if (sound_timer == 1)
				printf("BEEP!\n");
			--sound_timer;
		}
	}
}

void Chip8::debugRender()
{
	// Draw
	for (int y = 0; y < 32; ++y)
	{
		for (int x = 0; x < 64; ++x)
		{
			if (gfx[(y * 64) + x] == 0)
				printf("O");
			else
				printf(" ");
		}
		printf("\n");
	}
	printf("\n");
}