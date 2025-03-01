#include "BASS/bass.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"
#include "SDL2/SDL_messagebox.h"
#include "bassmanager.h"
#include <string>

using namespace BASSManager;

/// <summary>
/// A function to initialize the BASS library and plugins, prepare for use, and handle error checking.
/// Returns true on successful initialization, false on failure.
/// Takes the directory to the game as an input, to avoid header collisions (BASS headers include Windows headers that redefine HSPRITE)
/// </summary>
bool BASSManager::Initialize(const char* gameDir)
{
	std::string pluginPath;

	// Get window handle directly from SDL
	SDL_SysWMinfo sysInfo;
	SDL_VERSION(&sysInfo.version);

	SDL_Window* win = SDL_GL_GetCurrentWindow();
	SDL_bool infoSuccess = SDL_GetWindowWMInfo(win, &sysInfo);

	if (win != nullptr && infoSuccess == SDL_TRUE)
	{
		HWND hWindow = sysInfo.info.win.window;

		// TODO: Find if I can use client audio settings as flags
		if (!(bool)BASS_Init(-1, 44100, BASS_DEVICE_STEREO | BASS_DEVICE_16BITS, hWindow, nullptr))
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error", "BASS not initialized.", nullptr);
			return false;
		}
		else
		{
			// TODO: Wrap in ifdef so we can disable plugin loading
			#ifdef _WIN32
				pluginPath = std::string(gameDir) + "/cl_dlls/basszxtune.dll";
			#elif __linux__
				pluginPath = std::string(gameDir) + "/cl_dlls/libbasszxtune.so";
			#endif

			if (BASS_PluginLoad(pluginPath.c_str(), 0) != 0)
			{
				return true;
			}

			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error", "Failed to load BASS plugin.", nullptr);
			return false;
		}
	}

	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error", "Failed to obtain window handle.", nullptr);
	return false;
}

/// <summary>
/// Releases BASS resources to prepare for shutdown.
/// </summary>
void BASSManager::Shutdown()
{
	BASS_Free();
}

/// <summary>
/// Plays a song supported by BASS native file support or plugins such as ZXTune
/// </summary>

void BASSManager::PlaySong()
{
	// load the audio file as a stream
	HSTREAM stream = BASS_StreamCreateFile(FALSE, "filename.c_str()", 0, 0, 0);
	if (!stream)
	{

	}

	// play the stream
	if (!BASS_ChannelPlay(stream, FALSE))
	{

	}
}
