/*
	BASS Manager class for Half-Life Enhanced SDK
*/

#include "BASS/bass.h"
#include <filesystem_utils.cpp>

#pragma once

class CBASSManager
{
public:
	static CBASSManager& GetInstance()
	{
		return BASS_Instance;
	}

private:
	CBASSManager()
	{
		// TODO: Figure out if we can extract audio settings from the client
		if (!(bool)BASS_Init(-1, 44100, BASS_DEVICE_16BITS | BASS_DEVICE_STEREO, GetForegroundWindow(), nullptr))
		{
			ALERT(at_console, "BASS failed to initialize with error %d", BASS_ErrorGetCode());
		}
		else
		{
			ALERT(at_console, "BASS initialized successfully!");
		}
	}
	~CBASSManager()
	{
		BASS_Free();
	}

	static CBASSManager CBASSManager::BASS_Instance;
};