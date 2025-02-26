/*
	BASS Manager class for Half-Life Enhanced SDK
*/

#include "BASS/bass.h"
#include <filesystem_utils.cpp>

#pragma once

/// <summary>
/// A singleton to serve as a wrapper/abstraction layer for the BASS API,
///	to prevent header conflicts, manage resources efficiently, and
/// simplify ease-of-use when working with BASS.
/// </summary>
class CBASSManager
{
public:
	// copy constructor guard (ensure use reference/avoid creating copies)
	CBASSManager(const CBASSManager&) = delete;

	/// <summary>
	/// Returns the BASS manager object.
	/// Use with CBASSManager::GetInstance(), and then access the exposed function members as normal.
	/// For instance, CBASSManager::GetInstance().Play("example.mp3");
	/// </summary>
	/// <returns></returns>
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