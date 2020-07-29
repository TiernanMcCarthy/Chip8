#include "Core.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//0x000 - 0x1FF - Chip 8 interpreter(contains font set in emu)
//0x050 - 0x0A0 - Used for the built in 4x5 pixel font set(0 - F)
//0x200 - 0xFFF - Program ROM and work RAM
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





Core::Core()
{
	
}

void Core::Initialise()
{
	PC = 0x200; //Program counter at 0x200
	Opcode = 0; //Reset current opcode
	I = 0; //Reset index register 
	sp = 0; //Reset stack pointer

	srand(time(NULL));

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
		key[i] = V[i] = 0;
	}
	// Clear memory
	for (int i = 0; i < 4096; i++)
	{
		Memory[i] = 0;
	}
	// Load fontset into the first 80 bytes of memory
	for (int i = 0; i < 80; ++i)
		Memory[i] = chip8_fontset[i]; //Define font set soon


	// Load fontset  //0x50 == 80 and onwards
	for (int i = 0; i < 80; ++i)
		Memory[i] = chip8_fontset[i];


	//reset Timers
	delay_timer = 0;
	sound_timer = 0;

	DrawFlag = true;

	//ClearScreen = true;
}


//V[(Opcode & 0x0F00) >> 8] VX
//V[(Opcode & 0x00F0) >> 4] VY

//Function Pointers would be a much more intelligent approach for this
void Core::Emulate()
{
	//Fetch

	//Example 0xA2 + 0xF0 = 0xA2F0
	Opcode = Memory[PC] << 8 | Memory[PC + 1]; //Shift the current opcode 8 bits to the left and add the successive second byte of the opcode to this space

	if ((Opcode & 0xF000) == 0xF000)
	{
		int bob = 6;
	}
	//Decode
	switch (Opcode & 0xF000)
	{
		//0xANNN
	case 0xA000: //Sets I to the address NNN
		//Execute opcode
		I = Opcode & 0x0FFF;
		PC += 2;
		break;

	case 0xB000: //0xBNNN Jump to address NNN+V0;
		PC =  (Opcode & 0x0FFF) + V[0];
		break;

	case 0xC000: //0xCXNN Sets VX to the result of a bitwise and operation on a random number between 0 and 255 and NN
		V[(Opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (Opcode & 0x00FF);
		PC += 2;
		break;

	case 0x0000:
		switch (Opcode & 0x000F)
		{
		
		case 0x0000: //Clears the screen
		{
			for (int i = 0; i < 2048; ++i)
				gfx[i] = 0x0;
			DrawFlag = true;
			PC += 2;
		}
		//0x00EE
		case 0x000E: // 0x00EE: Returns from subroutine
			--sp;			// 16 levels of stack, decrease stack pointer to prevent overwrite
			PC = stack[sp];	// Put the stored return address from the stack back into the program counter					
			PC += 2;		// Don't forget to increase the program counter!
			break;

		default:
			printf("Unknown opcode [0x0000]: 0x%X\n", Opcode);
		}
		break;


	

	case 0xD000: // 0xDXYN   Draw a sprite at coordinate VX and VY and that has a width of 8 pixels and a height of n pixels
	{
		unsigned short x = V[(Opcode & 0x0F00) >> 8];
		unsigned short y = V[(Opcode & 0x00F0) >> 4];
		unsigned short height = Opcode & 0x000F;
		unsigned short pixel;

		V[0xF] = 0;
		for (int yline = 0; yline < height; yline++)
		{
			pixel = Memory[I + yline];
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

		DrawFlag = true;
		PC += 2;

		break;


	}


	case 0x1000: //0x1NNN: Jumps to address NNN
		PC = Opcode & 0x0FFF;
		break;

	case 0x2000: //0x2NNN call the subroutine at address NNN
		stack[sp] = PC;
		++sp;
		PC = Opcode & 0x0FFF;
		break;

	case 0x3000: //0x3NNN skip the next instruction if VX equals NN
		if (V[(Opcode & 0x0F00) >> 8] == (Opcode & 0x00FF))
		{
			PC += 4;
		}
		else
			PC += 2;
		break;

	case 0x4000: //0x4NNN skips the next instruction if VX doesn't equal NN
		if (V[(Opcode & 0x0F00) >> 8] != (Opcode & 0x00FF))
		{
			PC += 4;
		}
		else
			PC += 2;
		break;

	case 0x5000: //0x5XY0 skips the next instruction if VX equals VY
		if (V[(Opcode & 0x0F00) >> 8] == V[(Opcode & 0x00F0) >> 4])
		{
			PC += 4;
		}
		else
			PC += 2;
		break;
	case 0x6000: // 0x6XNN: Sets VX to NN.
		V[(Opcode & 0x0F00) >> 8] = Opcode & 0x00FF;
		PC += 2;
		break;

	case 0x7000: // 0x7XNN: Adds NN to VX.
		V[(Opcode & 0x0F00) >> 8] += Opcode & 0x00FF;
		PC += 2;
		break;

	case 0x8000: //All 8XY0-E Opcodes
		switch (Opcode & 0x000F)
		{

		case 0x0000: // 8XY0 Sets the value of VX to the value of VY
			V[(Opcode & 0x0F00) >> 8] = V[(Opcode & 0x00F0) >> 4];
			PC += 2;
			break;

		case 0x0001: // 8XY0 Sets the value of VX to VX or VY
			V[(Opcode & 0x0F00) >> 8] |= V[(Opcode & 0x00F0) >> 4];
			PC += 2;
			break;

		case 0x0002: // 8XY0 Sets the value of VX to VX and VY
			V[(Opcode & 0x0F00) >> 8] &= V[(Opcode & 0x00F0) >> 4];
			PC += 2;
			break;

		case 0x0003: // 8XY0 Sets the value of VX to VX Xor VY
			V[(Opcode & 0x0F00) >> 8] ^= V[(Opcode & 0x00F0) >> 4];
			PC += 2;
			break;

		case 0x0004: // 0x8XY4  Add the value of VY to VX
			if (V[(Opcode & 0x00F0) >> 4] > (0xFF - V[(Opcode & 0x0F00) >> 8]))  //Greater than 255 then carry
				V[0xF] = 1; //Carryflag 0XF just means the 15th register
			else
				V[0xF] = 0;
			V[(Opcode & 0x0F00) >> 8] += V[(Opcode & 0x00F0) >> 4];
			PC += 2;
			break;


		case 0x0005: // 0x8XY5  Subtract the value of VY from V
			if (V[(Opcode & 0x00F0) >> 4] > V[(Opcode & 0x0F00) >> 8])  //Less than other then borrow
				V[0xF] = 0; //Carryflag 0XF just means the 15th register
			else
				V[0xF] = 1;
			V[(Opcode & 0x0F00) >> 8] -= V[(Opcode & 0x00F0) >> 4];
			PC += 2;
			break;

		case 0x0006:  //0x8XY6  Stores the least significant bit of VX in VF and then shifts VX to the right by 1
			V[0xf] = V[(Opcode & 0x0F00) >> 8] & 0x1; 
			V[(Opcode & 0x0F00) >> 8] >>= 1;
			PC += 2;
			break;
			//0x8XY7 Sets VX to VY-VX;
		case 0x0007: // 0x8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't
			if (V[(Opcode & 0x0F00) >> 8] > V[(Opcode & 0x00F0) >> 4])	// VY-VX
				V[0xF] = 0; // there is a borrow
			else
				V[0xF] = 1;
			V[(Opcode & 0x0F00) >> 8] = V[(Opcode & 0x00F0) >> 4] - V[(Opcode & 0x0F00) >> 8];	
			PC += 2;
			break;


		case 0x000E: //0x8XYE Stores the most significant bit of VX in VF and then shifts VX to the left by 1
			V[0xF] = V[(Opcode & 0x0F00) >> 8] >> 7;
			V[(Opcode & 0x0F00) >> 8] <<= 1;
			PC += 2;
			break;

		default:
			printf("Unknown opcode [0x8000]: 0x%X\n", Opcode);

		}
		break;
	

	case 0x9000: //0x9XY0 Skips the next instruction if VX doesn't equal VY
		if (V[(Opcode & 0x0F00) >> 8] != V[(Opcode & 0x00F0) >> 4])
		{
			PC += 4;
			break;
		}
		else
		{
			PC += 2;
		}
		break;
		



	
		//More Opcodes over time

	case 0xE000: //Controller Input Section
		switch (Opcode & 0x00FF)
		{
			// EX9E: Skips the next instruction 
			// if the key stored in VX is pressed
			case 0x009E:
				if (key[V[(Opcode & 0x0F00) >> 8]] != 0)
					PC += 4;
				else
					PC += 2;
				break;
			
			case 0x00A1: //Skips the next instruction if the key in VX isn't pressed
				if (key[V[(Opcode & 0x0F00) >> 8]] == 0)
					PC += 4;
				else
					PC += 2;
				break;
		
			
			default:
				printf("Unknown opcode [0xE000]: 0x%X\n", Opcode);
		}
		break;

	
	case 0xF000: //Opcodes without a significant figure at 0xA <--

		switch (Opcode & 0x00FF)
		{
			//0x00E0

		case 0x0007: //0xFX07 Sets VX to the value of the delay timer
		{
			V[(Opcode & 0x0F00) >> 8] = delay_timer;
			PC += 2;
			break;
		}
		case 0x0015://FX15 Sets delay timer to VX
		{
			delay_timer = V[(Opcode & 0x0F00) >> 8];
			PC += 2;
			break;
		}
		case 0x0018://FX18 Sets Sound timer to VX
		{
			sound_timer = V[(Opcode & 0x0F00) >> 8];
			PC += 2;
			break;
		}
		case 0x001E: //FX1E Adds VX to I
		{
			I += V[(Opcode & 0x0F00) >> 8];
			PC += 2;
			break;
		}
		case 0x0029: //FX29 Sets I to the location of the sprite for the character in VX
		{
			I = V[(Opcode & 0x0F00) >> 8] * 0x5;
			PC += 2;
			break;
		}
		case 0x0055: //FX55 Stores v0 to VX in memory from address I
			for (int i = 0; i <= ((Opcode & 0x0F00) >> 8); i++)
			{
				Memory[I = i] = V[i];
			}

			//When the operation is done I = I + X+ 1
			I += ((Opcode & 0x0F00) >> 8) + 1;
			PC += 2;
			break;

		case 0x0065: // FX65: Fills V0 to VX with values from memory starting at address I					
			for (int i = 0; i <= ((Opcode & 0x0F00) >> 8); ++i)
				V[i] = Memory[I + i];

			// On the original interpreter, when the operation is done, I = I + X + 1.
			I += ((Opcode & 0x0F00) >> 8) + 1;
			PC += 2;
			break;

		case 0x000A: //0xFX0A if any key is pressed, resume, otherwise, halting is necessary
		{
			bool KeyPress = false;

			for (int i = 0; i < 16; i++)
			{
				if (key[0] == 1) //Believe a keypress is registered as 1
				{
					KeyPress = true;
				}
			}

			if (KeyPress)
			{
				PC += 2;
			}

			break;

		}




		case 0x0033: //0xFX33 Stores the binary coded decimal representation of VX at the address I, I +1, I +2
			Memory[I] = V[(Opcode & 0x0F00) >> 8] / 100;
			Memory[I + 1] = (V[(Opcode & 0x0F00) >> 8] / 10) % 10;
			Memory[I + 2] = (V[(Opcode & 0x0F00) >> 8] % 100) % 10;
			PC += 2;
			break;


		default:
			printf("Unknown opcode [0x0000]: 0x%X\n", Opcode);
			break;
		}

		break;
	default:
		printf("Unknown opcode: 0x%X\n", Opcode);


	break;
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


	//Execute


	//Update Timers
}



//void Core::Emulate()
//{
	//Fetch Opcode
	//Opcode = Memory[PC] << 8 | Memory[PC + 1];


//}

bool Core::LoadGame(const char* Program)
{
	printf("Loading Game File");

	printf(Program, "\n");

	//Access the file in this set directory;

	FILE* ROM;
	fopen_s(&ROM, Program, "rb"); //Read binary, fopen is deprecated, this way works just the same

	if (ROM==NULL)
	{
		printf("File Error, possible this was not a valid Chip 8 ROM \n", stderr); //Notify this was not a ROM with an error
		return false;
	}

	//Check for the size of this ROM, and then allocate a buffer for it to be placed into memory
	fseek(ROM, 0, SEEK_END);

	long Size = ftell(ROM);

	rewind(ROM);

	printf("FileSize: %d\n", (int)Size); //Report the file size

	//Allocate memory to contain the whole file
	char* buffer = (char*)malloc(sizeof(char) * Size); //A char is one byte and therefor will allocate the correct size

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
		return false;
	}

	//Copy buffer into Chip8 Memory

	//512 Bytes are reserved for the rom

	if((4096-512)>Size)
	{
		for (int i = 0; i < Size; i++)
		{
			Memory[i + 512] = buffer[i]; 
		}

	}
	else
	{
		printf("Error: This is not a valid chip8 file, it exceeds the size limitations \n");
	}

	//close file and free the memory buffer

	fclose(ROM);

	free(buffer);

	printf("Loading Successful/n");

	return true;

}


unsigned char* Core::GetGfx()
{
	return gfx;
}
