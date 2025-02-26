/*
	BASS Manager namespace for Half-Life Enhanced SDK
*/

#pragma once

/// <summary>
/// A namespace to serve as a wrapper/abstraction layer for the BASS API,
///	to prevent header conflicts, manage resources efficiently, and
/// simplify ease-of-use when working with BASS.
/// </summary>
namespace BASSManager
{
bool Initialize(const char* gameDir);
void Shutdown();
void PlaySong();
};