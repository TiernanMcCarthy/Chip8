#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>
#include "Core.h"



sf::Color colours[4] = { sf::Color(255,0,0),sf::Color(0,255,0),sf::Color(0,0,255),sf::Color(255,0,255) };

using namespace std;
sf::Image Map;


 

#define Width 1280
#define Height 640
#define TileSize 20 //Tiles are square, x*y pixels e.t.c

bool stop = false;

void DrawSegment(sf::Image& map, int x, int y, sf::Color DrawColour)
{
	for (int Y = y; Y < y + TileSize; Y++)
	{
		for (int X = x; X < x + TileSize; X++)
		{
			map.setPixel(X, Y, DrawColour);
		}
	}

}


Core Chip8;
sf::Texture texture;
sf::Image Background;

void DrawChip()
{

	unsigned char* gfxarray = Chip8.GetGfx();

	for (int y = 0; y < 32; y++)
	{
		for (int x = 0; x < 64; x++)
		{
			if (gfxarray[(y * 64) + x] == 0)
			{
				DrawSegment(Background, x * TileSize, y * TileSize, sf::Color::Black);
			}
			else
			{
				DrawSegment(Background, x * TileSize, y * TileSize, sf::Color::White);
			}

		}

	}

}

/*

Recommended Keyboard layout

Keypad                   Keyboard
+-+-+-+-+                +-+-+-+-+
|1|2|3|C|                |1|2|3|4|
+-+-+-+-+                +-+-+-+-+
|4|5|6|D|                |Q|W|E|R|
+-+-+-+-+       =>       +-+-+-+-+
|7|8|9|E|                |A|S|D|F|
+-+-+-+-+                +-+-+-+-+
|A|0|B|F|                |Z|X|C|V|
+-+-+-+-+                +-+-+-+-+


*/

void KeyDown()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) Chip8.key[1] = 1;  //TopRow  E.G 1 2 3 4
	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) Chip8.key[2] = 1;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) Chip8.key[3] = 1;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) Chip8.key[12] = 1;

	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) Chip8.key[4] = 1;  //QWER
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) Chip8.key[5] = 1;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) Chip8.key[6] = 1;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) Chip8.key[13] = 1;

	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) Chip8.key[7] = 1;  //ASDF
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) Chip8.key[8] = 1;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) Chip8.key[9] = 1;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) Chip8.key[14] = 1;

	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) Chip8.key[10] = 1;  //ZXCV
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) Chip8.key[0] = 1;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) Chip8.key[11] = 1;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) Chip8.key[15] = 1;


}

void KeyUp()
{
	if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) && Chip8.key[1]==1) Chip8.key[1] = 0;  //TopRow  E.G 1 2 3 4
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) && Chip8.key[2] == 1) Chip8.key[2] = 0;
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Num3) && Chip8.key[3] == 1) Chip8.key[3] = 0;
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Num4) && Chip8.key[12] == 1) Chip8.key[12] = 0;

	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && Chip8.key[4] == 1) Chip8.key[4] = 0;  //QWER
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::W) && Chip8.key[5] == 1) Chip8.key[5] = 0;
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::E) && Chip8.key[6] == 1) Chip8.key[6] = 0;
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::R) && Chip8.key[13] == 1) Chip8.key[13] = 0;

	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::A) && Chip8.key[7] == 1) Chip8.key[7] = 0;  //ASDF
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::S) && Chip8.key[8] == 1) Chip8.key[8] = 0;
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::D) && Chip8.key[9] == 1) Chip8.key[9] = 0;
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::F) && Chip8.key[14] == 1) Chip8.key[14] = 0;

	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && Chip8.key[10] == 1) Chip8.key[10] = 0;  //ZXCV
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::X) && Chip8.key[0] == 1) Chip8.key[0] = 0;
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::C) && Chip8.key[11] == 1) Chip8.key[11] = 0;
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::V) && Chip8.key[15] == 1) Chip8.key[15] = 0;


}


int main()
{
	srand(time(NULL)); //seed for random function

	//Create SFML Window
	sf::RenderWindow window(sf::VideoMode(Width, Height), "Chip8 Tiernan");

	//Define a image for drawing sprites to
	Background.create(Width, Height);



	if (!texture.create(Width, Height))
	{
		printf("Texture failed");
	}




	//Ensure the image isn't filtered
	texture.setRepeated(false);
	texture.setSmooth(false);

	//Arbritrary speed, really should adjust emulation to run at the correct speed, seperate from the actual program frame rate
	int FrameSpeed = 600; //Look closer into this, games on the chip 8 don't look great in the first place

	//Create a sprite to draw the scene onto
	sf::Sprite sprite;
	sprite.setTexture(texture);


	window.setFramerateLimit(FrameSpeed);// limit frame rate to a reasonable level

	bool START = false;

	//Initialise CHIP8 Memory and key components
	Chip8.Initialise();
	//Load the contents of a ROM into the CHIP-8 RAM
	Chip8.LoadGame("pong2.c8"); //This was done all in one go in CHIP-8, these games are only a few hundred bytes
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed)
			{
				KeyDown();
			}
			if (event.type == sf::Event::KeyReleased)
			{
				KeyUp();
			}

		


		}

		Chip8.Emulate();
		if (Chip8.DrawFlag)
		{
			DrawChip();
			//Tell the CHIP8 the frame has been drawn
			Chip8.DrawFlag = false; 
		}

		//Clear and set the background for rendering
		window.clear();
		texture.update(Background);
		sprite.setTexture(texture);
		window.draw(sprite);
		window.display();
	}

	return 0;
}