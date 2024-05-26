#pragma once

// Try look for main `logging.hpp` elsewhere.
#if __has_include("logging.hpp")
#	include "logging.hpp"
#elif defined(PLATFORMIO)
//	// From PlatformIO specific usual library locations, 
//	// try look for logging.hpp in main source or top-level directory:
//	// * external libraries; navigate from `.pio/libdeps/<board>/<library>/src` to `src/logging.hpp`
#	if __has_include("../../../../../src/logging.hpp")
#		include "../../../../../src/logging.hpp"
//	// * external libraries; navigate from `.pio/libdeps/<board>/<library>` to `src/logging.hpp`
#	elif __has_include("../../../../src/logging.hpp")
#		include "../../../../src/logging.hpp"
//	// * private libraries; navigate from `libs/<library>/src` to `src/logging.hpp`
#	elif __has_include("../../../src/logging.hpp")
#		include "../../../src/logging.hpp"
//	// * private libraries; navigate from `libs/<library>` to `src/logging.hpp`
#	elif __has_include("../../src/logging.hpp")
#		include "../../src/logging.hpp"
//	// * external libraries; navigate from `.pio/libdeps/<board>/<library>/src` to `logging.hpp` (root, next to the ini)
#	elif __has_include("../../../../../src/logging.hpp")
#		include "../../../../../src/logging.hpp"
//	// * external libraries; navigate from `.pio/libdeps/<board>/<library>` to `logging.hpp` (root, next to the ini)
#	elif __has_include("../../../../src/logging.hpp")
#		include "../../../../src/logging.hpp"
//	// * private libraries; navigate from `libs/<library>/src` to `logging.hpp` (root, next to the ini)
#	elif __has_include("../../../src/logging.hpp")
#		include "../../../src/logging.hpp"
//	// * private libraries; navigate from `libs/<library>` to `logging.hpp` (root, next to the ini)
#	elif __has_include("../../src/logging.hpp")
#		include "../../src/logging.hpp"
#	endif
#else
//	// It might be Arduino IDE, but there is no way to get to specific project/sketch project folder.
//	// It might be custom CMake/scripts/toolchain, those users most likely can figure out stuff for themselves.
//	// Anyway, let's try parent/grandparent folder.
#	if __has_include("../logging.hpp")
#		include "../logging.hpp" 
#	elif __has_include("../../logging.hpp")
#		include "../../logging.hpp" 
#	endif
#endif
