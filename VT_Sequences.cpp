#pragma region Boilerplate
#include "pch.h"
#include "framework.h"
#include <Windows.h>
#include <stdio.h>
#include <sstream>

///<summary> The escape sequence used for virtual terminal sequences. </summary>
#define ESC "\x1b"

//Variables
///<summary> Is the console's altenate buffer currently active? </summary>
bool AlternateBufferActive = false;
#pragma endregion

/// <summary> Modifies the console output mode to handle virtual sequences. Necessary to utilize any virtual sequences. </summary>
/// <returns> True if successful, otherwise returns false. </returns> 
static bool EnableVirtual()
{
	// Set output mode to handle virtual terminal sequences
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
	if (hIn == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DWORD dwOriginalOutMode = 0;
	DWORD dwOriginalInMode = 0;
	if (!GetConsoleMode(hOut, &dwOriginalOutMode))
	{
		return false;
	}
	if (!GetConsoleMode(hIn, &dwOriginalInMode))
	{
		return false;
	}

	DWORD dwRequestedOutModes = ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
	DWORD dwRequestedInModes = ENABLE_VIRTUAL_TERMINAL_INPUT;

	DWORD dwOutMode = dwOriginalOutMode | dwRequestedOutModes;
	if (!SetConsoleMode(hOut, dwOutMode))
	{
		// we failed to set both modes, try to step down mode gracefully.
		dwRequestedOutModes = ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		dwOutMode = dwOriginalOutMode | dwRequestedOutModes;
		if (!SetConsoleMode(hOut, dwOutMode))
		{
			// Failed to set any VT mode, can't do anything here.
			return false;
		}
	}

	DWORD dwInMode = dwOriginalInMode | dwRequestedInModes;
	if (!SetConsoleMode(hIn, dwInMode))
	{
		// Failed to set VT input mode, can't do anything here.
		return false;
	}

	return true;
}

/// <summary> Sets the console window and buffer size to the value specified by the given parameters. </summary>
/// <param name="X:"> The width of the console window in characters. </param>
/// <param name="Y:"> The height of the console window in characters. </param>
static void SetConsoleSize(int X, int Y)
{
	std::ostringstream command;
	command << ESC << "[8;" << X << ";" << Y << "t";
	printf(command.str().c_str());
}

/// <summary> Switches the active console buffer to the alternate buffer if the main buffer is active, otherwise returns to the main console buffer. </summary>
/// <param name="Clear:"> Optional, clears the switched-from buffer using the system("cls") call if set to true. </param>
/// <returns> True if the switched-to buffer was the alternate buffer, otherwise returns false. </returns>
static bool SwitchScreenBuffer(bool Clear = false)
{
	if (Clear) system("cls");
	if (!AlternateBufferActive)
	{
		printf(ESC "[?1049h");
		AlternateBufferActive = true;
		return true;
	}
	else
	{
		printf(ESC "[?1049l");
		AlternateBufferActive = false;
		return false;
	}
}

/// <summary> Sets the console foreground and background colors using the parameters called.</summary>
/// <param name="Table:"> [ 1: Black | 2: Red | 3: Green | 4: Yellow | 5: Blue | 6: Magenta | 7: Cyan | 8: White ]
/// Add 8 for a bright/bold version of any given color, use 0 to reset to the default color, or -1 to leave the color as is.</param>
/// <param name=""></param>
/// <param name="ForegroundColor:"> The color to set the foreground color to according to the color table attached. </param>
/// <param name="BackgroundColor:"> The color to set the background color to according to the color table attached. </param>
/// <returns> Returns true if the given color choices were set successfully, otherwise returns false. </returns>
static bool SetConsoleColor(int ForegroundColor, int BackgroundColor)
{
	switch (ForegroundColor)
	{
	default: return false;
	case -1: break; // Do Nothing
	case  0: printf(ESC "[39m"); break; // Reset
	case  1: printf(ESC "[30m"); break; // Black
	case  2: printf(ESC "[31m"); break; // Red
	case  3: printf(ESC "[32m"); break; // Green
	case  4: printf(ESC "[33m"); break; // Yellow
	case  5: printf(ESC "[34m"); break; // Blue
	case  6: printf(ESC "[35m"); break; // Magenta
	case  7: printf(ESC "[36m"); break; // Cyan
	case  8: printf(ESC "[37m"); break; // White
	case  9: printf(ESC "[90m"); break; // Bright Black
	case 10: printf(ESC "[91m"); break; // Bright Red
	case 11: printf(ESC "[92m"); break; // Bright Green
	case 12: printf(ESC "[93m"); break; // Bright Yellow
	case 13: printf(ESC "[94m"); break; // Bright Blue
	case 14: printf(ESC "[95m"); break; // Bright Magenta
	case 15: printf(ESC "[96m"); break; // Bright Cyan
	case 16: printf(ESC "[97m"); break; // Bright White
	}
	switch (BackgroundColor)
	{
	default: return false;
	case -1: break; // Do Nothing
	case  0: printf(ESC "[49m"); break; // Reset
	case  1: printf(ESC "[40m"); break; // Black
	case  2: printf(ESC "[41m"); break; // Red
	case  3: printf(ESC "[42m"); break; // Green
	case  4: printf(ESC "[43m"); break; // Yellow
	case  5: printf(ESC "[44m"); break; // Blue
	case  6: printf(ESC "[45m"); break; // Magenta
	case  7: printf(ESC "[46m"); break; // Cyan
	case  8: printf(ESC "[47m"); break; // White
	case  9: printf(ESC "[100m"); break; // Bright Black
	case 10: printf(ESC "[101m"); break; // Bright Red
	case 11: printf(ESC "[102m"); break; // Bright Green
	case 12: printf(ESC "[103m"); break; // Bright Yellow
	case 13: printf(ESC "[104m"); break; // Bright Blue
	case 14: printf(ESC "[105m"); break; // Bright Magenta
	case 15: printf(ESC "[106m"); break; // Bright Cyan
	case 16: printf(ESC "[107m"); break; // Bright White
	}

	return true;
}

/// <summary> Inverts the current console colors (Foreground color is set to background color and vice versa). </summary>
static void NegativeConsole()
{
	printf(ESC "[7m");
}