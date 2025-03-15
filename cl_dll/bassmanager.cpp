#include "BASS/bass.h"
#include "BASS/basszxtune.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"
#include "SDL2/SDL_messagebox.h"
#include <string>
#include <array>
#include <dlfcn.h>
#include <unistd.h>
#include <filesystem>

#include "bassmanager.h"
#include "cl_dll.h"

namespace BASSManager
{
	#ifdef __linux__
	// dlopen handle
	void* bassHnd;
	void* zxHnd;

	// * ideally we would have our function pointers here, but bassHnd needs to be set before we can create them.
	#endif

	/// @brief Displays a BASS error code with the failed library path for path debugging.
	/// @param errorFile 
	void ErrorDisplay(std::filesystem::path errorFile)
	{
		auto BASSError = reinterpret_cast<decltype(BASS_ErrorGetCode)*>(dlsym(bassHnd, "BASS_ErrorGetCode"));

		char errBuf[384];
		snprintf(errBuf, sizeof(errBuf), "BASS Error %d @ %s", BASSError(), errorFile.c_str());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error", errBuf, nullptr);
	}

	#ifdef _WIN32
	/// <summary>
	/// A function to initialize the BASS library and plugins, prepare for use, and handle error checking.
	/// Returns true on successful initialization, false on failure.
	/// Takes the directory to the game as an input, to avoid header collisions (BASS headers include Windows headers that redefine HSPRITE)
	/// Logic is different for Linux and Windows, but automatically handled with ifdefs.
	/// </summary>
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
	/// @brief Initialize BASS, import functions and plugins, and setup paths
	/// @param modDir 
	/// @return true on successful load, false otherwise
	bool Initialize(const char* modDir)
	{
		// library paths
		std::filesystem::path fullDir = std::filesystem::current_path() / modDir;

		std::filesystem::path bassPath = fullDir / "cl_dlls/libbass.so";

		// * WARNING! If you want to use BASSZXTune on Linux, the .so file doesn't look for dependencies within it's own folder.
		// * To fix this, use the command
		// * patchelf --set-rpath '$ORIGIN' /path/to/libbasszxtune.so and verify that libbass is found with ldd to patch it.
		std::filesystem::path zxPath = fullDir / "cl_dlls/libbasszxtune.so";

		// set bass handle
		bassHnd = dlopen(bassPath.c_str(), RTLD_NOW);

		auto BASSInit = reinterpret_cast<decltype(BASS_Init)*>(dlsym(bassHnd, "BASS_Init"));
		auto BASSPluginLoad = reinterpret_cast<decltype(BASS_PluginLoad)*>(dlsym(bassHnd, "BASS_PluginLoad"));
		auto BASSStart = reinterpret_cast<decltype(BASS_IsStarted)*>(dlsym(bassHnd, "BASS_IsStarted"));
		
		// simplified initialization logic as Linux doesn't require window pointer.
		if (!BASSInit(-1, 44100, BASS_DEVICE_STEREO | BASS_DEVICE_16BITS, nullptr, nullptr))
		{
			ErrorDisplay(bassPath);
			Shutdown();
			return false;
		}
		else
		{
			if(std::filesystem::exists(zxPath));
			{
				// zxHnd = dlopen(zxPath.c_str(), RTLD_NOW);

				// todo: make this more modular and flexible again
				HPLUGIN plugin = BASSPluginLoad(zxPath.c_str(), 0);
				if (!plugin)
				{
					ErrorDisplay(zxPath);
					return false;
				}
			}
			// load plugins
			// if(numPlugins > 0)
			// {
			// 	for(int i = 0; i < numPlugins; i++)
			// 	{
			// 	}
			// }
			PlaySong();
			return true;
		}
	}
	#endif

	/// <summary>
	/// Releases BASS resources to prepare for shutdown.
	/// </summary>
	void Shutdown()
	{
		#ifdef __linux__
		auto BASSFree = reinterpret_cast<decltype(BASS_Free)*>(dlsym(bassHnd, "BASS_Free"));

		BASSFree();
		dlclose(bassHnd);
		#endif
	}

	/// <summary>
	/// Plays a song supported by BASS native file support or plugins such as ZXTune
	/// </summary>
	void PlaySong()
	{
		#ifdef __linux__
		// * This function works even with plugins.
		auto BASSStreamCreate = reinterpret_cast<decltype(BASS_StreamCreateFile)*>(dlsym(bassHnd, "BASS_StreamCreateFile"));
		auto BASSMod = reinterpret_cast<decltype(BASS_MusicLoad)*>(dlsym(bassHnd, "BASS_MusicLoad"));
		auto BASSPlay = reinterpret_cast<decltype(BASS_ChannelPlay)*>(dlsym(bassHnd, "BASS_ChannelPlay"));
		
		// load the audio file as a stream
		HSTREAM stream = BASSStreamCreate(FALSE, "", 0, 0, 0);
		BASSPlay(stream, FALSE);
		#endif
	}
}
