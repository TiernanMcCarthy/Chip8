
#include "Chip8.h"
#include <stdio.h>
#include <stdlib.h>

//OpenGL
#include <windows.h>
#include <math.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <atlstr.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#pragma region  OpenGlShite
/////////////////////
// GLOBALS
//
/// Define a structure to hold all
/// of the global variables of this app.
struct Globals
{
	HINSTANCE hInstance;    // window app instance

	HWND hwnd;      // handle for the window

	HDC   hdc;      // handle to device context

	HGLRC hglrc;    // handle to OpenGL rendering context

	int width, height;      // the desired width and
	// height of the CLIENT AREA
	// (DRAWABLE REGION in Window)
};

///////////////////////////
// GLOBALS
// declare one struct Globals called g;
Globals g;
//
///////////////////////////


// Function prototypes.
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow);
void draw();            // drawing function containing OpenGL function 

#pragma endregion






























Chip8 myChip8; //Create the CPU core as a class

#define TestState 0 //1 = testing 0 = not
void SetupGraphics()
{

}
void SetupInput()
{

}

void DrawGraphics()
{

}



/*int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	LPWSTR *szArgList;
	int argCount;
	//Tutorial is just getcommandline() but that doesn't work
	szArgList = CommandLineToArgvW(GetCommandLineW(), &argCount);
	if (szArgList==NULL)
	{
		MessageBox(NULL, "Unable to pass command line", "Error", MB_OK);
	}
	for (int i = 0; i < argCount; i++)
	{
	     MessageBoxW(NULL, szArgList[i], L"Arglist contents", MB_OK);
	}
	LocalFree(szArgList);
	return 0;
}*/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	LPWSTR *szArgList; //First Argument is the exe of this program itself, the following are anything commandline supplies it
	int argCount;
	//Tutorial is just getcommandline() but that doesn't work
	szArgList = CommandLineToArgvW(GetCommandLineW(), &argCount);
	if (szArgList == NULL)
	{
		MessageBox(NULL, "Unable to pass command line", "Error", MB_OK);
	}
	for (int i = 0; i < argCount; i++)
	{
		MessageBoxW(NULL, szArgList[i], L"Arglist contents", MB_OK);
	}
	CStringA sB(szArgList[1]); //Convert this argument into a const char for loading the file
	const char* pszC = sB;
	char* pszD = const_cast<char*>(pszC);
	LocalFree(szArgList);
	//return 0;
	//Setup Render systems and input
	SetupGraphics();
	SetupInput();
	myChip8.initialize();
	if(TestState==1) //Just launch the one game
	{ 
		myChip8.LoadProgram("F:/Chip8Tiernan/Debug/invaders.c8");
	}
	else {

		if (szArgList[1]!=NULL)
		{
			if (myChip8.LoadProgram(pszD)==false)
			{
				MessageBox(NULL, "Not a valid file", "Error", MB_OK);
				return 1;
			}
		}
	}


	//Emulation Loop
	int once = 0;
	for (;;) //forever
	{
		//emulate one cylce
		myChip8.emulateCycle();
		
		//If the draw flag is set update the screen

		if (myChip8.drawflag == true&&once==0)
		{
			myChip8.debugRender();
			//once = 1;
		}
		//return 0;
			//DrawGraphics();

		//Store the key press state (press and release)
		//myChip8.setKeys();

	}
	return 0;
}