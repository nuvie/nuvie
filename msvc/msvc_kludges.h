//
// MSVC Kludges... and precompiled header for Nuvie
//
// DO NOT INCLUDE
//
// This files allows Nuvie to compile under the very non
// standards compilent MSVC. It also serves as a precompiled header to greatly
// improve compile times in MSVC.
//

#ifndef MSVC_KLUDGES_H
#define MSVC_KLUDGES_H


// Defines needed by Nuvie
// Don't want SDL Parachute
//#define NO_SDL_PARACHUTE


// Disable some warnings
#pragma warning (disable: 4786)  // Debug Len > 255
#pragma warning (disable: 4275)  // non dll-interface class ...

// Include that before all other things; caused problems in wchar.h
#include <string>
#include <math.h>

#define snprintf _snprintf
#define strcasecmp _stricmp


// Using STLport?
#ifdef _STLP_MSVC

	// Yes, we have STLport

	// Put some symbols back into the std namespace
	#include <cstdlib>
	#include <cctype>

	namespace std
	{
	   using _STLP_VENDOR_CSTD::labs;
	   using _STLP_VENDOR_CSTD::toupper;
	}


#else // _STLP_MSVC

	// No, we don't have STLport


	// Put the headers that should be in std into the std namespace
	namespace std {

	   #include <cstdio>
		#include <cstdlib>
		#include <cstring>
		#include <ctime>
		#include <cmath>
		#include <cstdarg>
	   #include <cctype>

	   using ::strlen;
	   using ::toupper;
	   using ::size_t;

		using ::snprintf;
		using ::labs;
	}

#endif // _STLP_MSVC

// Precompiled Header part
#ifdef USING_PRECOMPILED_HEADER

	// Some often used headers that could be included in our precompiled header
	#include <fstream>
	#include <exception>
	#include <vector>
	#include <iostream>
	#include <iomanip>
	#include <set>
	#include <map>

	#include <SDL.h>

	// nuvie-related headers that don't change often
	//#include "U6File.h"

#endif //USING_PRECOMPILED_HEADER


#endif /* !MSVC_KLUDGES_H */
