#include "BASS/bass.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"
#include "SDL2/SDL_messagebox.h"
#include <string>
#include <dlfcn.h>
#include <vector>

#include "bassmanager.h"

namespace BASSManager
{
	#ifdef __linux__
	void* bassHnd;
	#endif

	/// <summary>
	/// A function to initialize the BASS library and plugins, prepare for use, and handle error checking.
	/// Returns true on successful initialization, false on failure.
	/// Takes the directory to the game as an input, to avoid header collisions (BASS headers include Windows headers that redefine HSPRITE)
	/// Logic is different for Linux and Windows, but automatically handled with ifdefs.
	/// </summary>
	#ifdef _WIN32
	bool Initialize(const char* gameDir)
	{
		std::string pluginPath;

		// Get window handle directly from SDL
		SDL_SysWMinfo sysInfo;
		SDL_VERSION(&sysInfo.version);

		// Getting the OpenGL window, as that actually is what the game is running from.
		SDL_Window* win = SDL_GL_GetCurrentWindow();
		SDL_bool infoSuccess = SDL_GetWindowWMInfo(win, &sysInfo);

		if (win != nullptr && infoSuccess == SDL_TRUE)
		{
			// TODO: Find if I can use client audio settings as flags
			if (!(bool)BASS_Init(-1, 44100, BASS_DEVICE_STEREO | BASS_DEVICE_16BITS, sysInfo.info.win.window, nullptr))
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error", "BASS not initialized.", nullptr);
				return false;
			}
			else
			{
				pluginPath = std::string(gameDir) + "/cl_dlls/basszxtune.dll";

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
	#elif __linux__
	bool Initialize(const char* gameDir)
	{
		// library paths
		std::vector<std::string> pluginPaths = { "/cl_dlls/libbasszxtune.so" };
		std::string bassPath = std::string(gameDir) + "/cl_dlls/libbass.so";

		// set bass handle
		bassHnd = dlopen(bassPath.c_str(), RTLD_GLOBAL | RTLD_LAZY);

		// get bass fn pointers from libbass.so
		auto BASSInit = (decltype(BASS_Init)*)dlsym(bassHnd, "BASS_Init");
		auto BASSPluginLoad = (decltype(BASS_PluginLoad)*)dlsym(bassHnd, "BASS_PluginLoad");

		// simplified initialization logic as Linux doesn't require window pointer.
		if (!(bool)BASSInit(-1, 44100, BASS_DEVICE_STEREO | BASS_DEVICE_16BITS, nullptr, nullptr))
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error", "BASS not initialized.", nullptr);
			return false;
		}
		else
		{
			// check plugin vector and loop through
			if(pluginPaths.size() > 0)
			{
				for(int i = 0; i < pluginPaths.size(); i++)
				{
					if (!BASSPluginLoad((std::string(gameDir) + pluginPaths[i]).c_str(), 0) != 0)
					{
						SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error", "Failed to load BASS plugin.", nullptr);
						return false;
					}
				}
				return true;
			}
		}
	}
	#endif

	/// <summary>
	/// Releases BASS resources to prepare for shutdown.
	/// </summary>
	void Shutdown()
	{
		#ifdef __linux__
		auto BASSFree = (decltype(BASS_Free)*)dlsym(bassHnd, "BASS_Free");
		BASSFree();

		dlclose(bassHnd);
		#endif
	}

	/// <summary>
	/// Plays a song supported by BASS native file support or plugins such as ZXTune
	/// </summary>
	void PlaySong()
	{
		// load the audio file as a stream
		/* HSTREAM stream = BASS_StreamCreateFile(FALSE, "filename.c_str()", 0, 0, 0);
		if (!stream)
		{

		}

		// play the stream
		if (!BASS_ChannelPlay(stream, FALSE))
		{

		} */
	}
}
