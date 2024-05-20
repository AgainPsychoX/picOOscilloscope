#pragma once

#if __has_include(<cstdint>) 
#	include <cstdint> 
#else 
#	include <stdint.h>
#endif 

#if defined(LOG_DISABLE)
#	define LOG_PRINTLN(...) 
#	define LOG_PRINTF(...) 
#endif

#if defined(LOG_USE_STDOUT)
#	include <cstdio>
#	define LOG_PRINTF(...) printf(__VA_ARGS__)
#	define LOG_PRINTLN(s) puts(s);
#endif

#if defined(LOG_PRINTF)
#	if not defined(LOG_PRINTLN)
#		define LOG_PRINTLN(s) LOG_PRINTF(s "\n");
#	endif
#endif

#if defined(ARDUINO) and not defined(LOG_ARDUINO_PORT)
#	define LOG_ARDUINO_PORT Serial// Arduino `Print`-compatible target for logging
#endif

////////////////////////////////////////////////////////////////////////////////
// Espressif family

#if defined(ARDUINO_ARCH_ESP32) and not defined(LOG_PRINTF)
#	include <Arduino.h>
#	define LOG_PRINTF(...) LOG_ARDUINO_PORT.printf(__VA_ARGS__)
#	define LOG_PRINTLN(s) LOG_ARDUINO_PORT.println(s);
#endif
#if defined(ARDUINO_ARCH_ESP8266) and not defined(LOG_PRINTF)
#	include <Arduino.h>
#	define LOG_PRINTF(fmt, ...) LOG_ARDUINO_PORT.printf_P(PSTR(fmt), __VA_ARGS__)
#	define LOG_PRINTLN(s) LOG_ARDUINO_PORT.println(F(s));
#endif
#if (defined(ESP32) or defined(ESP8266)) and not defined(LOG_PRINTF)
#	if defined(LOG_USE_ESPIDF)
// Important: When using ESP-IDF, you might want to use `esp_log_level_set` 
// to set the log level to the lowest most detailed level that you want to log.
#		include <esp_log.h>
#		define LOG_ERROR(tag, ...) if (CHECK_LOG_LEVEL(tag, LOG_LEVEL_ERROR)) ESP_LOGE(__VA_ARGS__)
#		define LOG_WARN(tag, ...)  if (CHECK_LOG_LEVEL(tag, LOG_LEVEL_WARN)) ESP_LOGW(__VA_ARGS__)
#		define LOG_INFO(tag, ...)  if (CHECK_LOG_LEVEL(tag, LOG_LEVEL_INFO)) ESP_LOGI(__VA_ARGS__)
#		define LOG_DEBUG(tag, ...) if (CHECK_LOG_LEVEL(tag, LOG_LEVEL_DEBUG)) ESP_LOGD(__VA_ARGS__)
#		define LOG_TRACE(tag, ...) if (CHECK_LOG_LEVEL(tag, LOG_LEVEL_TRACE)) ESP_LOGV(__VA_ARGS__)
#	endif
#endif

////////////////////////////////////////////////////////////////////////////////
// Generic Arduino, incl. Arduino-Pico

#if defined(ARDUINO) and not defined(LOG_PRINTF)
#include <Arduino.h>

namespace {
	template <typename T>
	class has_printf_P
	{
		template <typename C> static char test(decltype(&C::printf_P)) ;
		template <typename C> static char* test(...);
	public:
		static constexpr bool value = sizeof(test<T>(0)) == sizeof(char);
	};
	template <typename T>
	class has_printf
	{
		template <typename C> static char test(decltype(&C::printf));
		template <typename C> static char* test(...);
	public:
		static constexpr bool value = sizeof(test<T>(0)) == sizeof(char);
	};

	template <int> struct tag { };

	template <typename C, typename... Args>
	inline __attribute__((always_inline)) void invoke(tag<0>, C& o, PGM_P fmt, Args ...args) 
	{
		// Just print format as fallback
		o.println(reinterpret_cast<const __FlashStringHelper *>(fmt));
	}
	template <typename C, typename... Args>
	inline __attribute__((always_inline)) void invoke(tag<1>, C& o, PGM_P fmt, Args ...args)
	{
		o.printf_P(fmt, args...);
	}
	template <typename C, typename... Args>
	inline __attribute__((always_inline)) void invoke(tag<2>, C& o, PGM_P fmt, Args ...args)
	{
		o.printf(reinterpret_cast<const __FlashStringHelper *>(fmt), args...);
	}
}

/// Util to invoke printf_P or printf on the Serial/Print class (if exists), 
/// fallback to println (if doesn't exist). Used in logging macros.
template <typename C, typename... Args>
inline __attribute__((always_inline)) void printf_if_exists(C& o, PGM_P fmt, Args ...args)
{
	invoke(tag<has_printf_P<C>::value ? 1 :
	/*********/has_printf<C>::value ? 2 : 0>{}, o, fmt, args...);
}

#define LOG_PRINTF(fmt, ...) printf_if_exists(LOG_ARDUINO_PORT, PSTR(fmt), __VA_ARGS__)
#define LOG_PRINTLN(...) LOG_ARDUINO_PORT.println(F(__VA_ARGS__))

#endif // defined(ARDUINO) and not defined(LOG_PRINTF)

////////////////////////////////////////////////////////////////////////////////
// Fallback to stdout

#if not defined(LOG_PRINTF)
#	include <cstdio>
#	define LOG_PRINTF(...) printf(__VA_ARGS__)
#	define LOG_PRINTLN(s) puts(s);
#endif

////////////////////////////////////////////////////////////////////////////////
// Logging implementation

#define LOG_LEVEL_NONE  0	// No logging to be performed.
#define LOG_LEVEL_ERROR 1	// Critical errors, software module can not recover on its own.
#define LOG_LEVEL_WARN  2	// Expected error conditions from which recovery measures have been taken.
#define LOG_LEVEL_INFO  3	// Information messages which describe normal flow of events; worth taking notice.
#define LOG_LEVEL_DEBUG 4	// Extra information which is not necessary for normal use (conditions met, values, pointers, sizes, etc).
#define LOG_LEVEL_TRACE 5	// Bigger chunks of debugging information, or frequent messages which can potentially flood the output.

// FNV1a 32 (source: https://gist.github.com/Lee-R/3839813, license: Public Domain)
constexpr uint32_t fnv1a_32_recursive(const char* const s, unsigned int count) {
	return count ? (fnv1a_32_recursive(s, count - 1) ^ s[count - 1]) * 16777619u : 2166136261u;
}

template <unsigned int N>
constexpr uint32_t hashLogTagStr(const char (&str) [N]) {
	return fnv1a_32_recursive(str, N - 1);
}

/// Defines default log level, MUST BE SET ONCE.
#define USE_LOG_LEVEL_DEFAULT(level) template <uint64_t str_hash> struct LogLevelForTag { static constexpr int value = level; };

/// Defines log level for specific tag.
#define USE_LOG_LEVEL(tag, level) template <> struct LogLevelForTag<hashLogTagStr(tag)> { static constexpr int value = level; }

/// Returns log level for specific tag.
#define GET_LOG_LEVEL(tag) LogLevelForTag<hashLogTagStr(tag)>::value

/// Condition for whenever specific tag with specific level should be logged
/// in current configuration. Useful to omit off logging specific/support code.
#ifdef LOG_DISABLE
#	define CHECK_LOG_LEVEL(tag, level) (0)
#else
#	define CHECK_LOG_LEVEL(tag, level) (GET_LOG_LEVEL(tag) >= level)
#endif

#define _LOG_IMPL_GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, N,...) N
#define _LOG_IMPL(...) _LOG_IMPL_GET_MACRO(__VA_ARGS__, LOG_V, LOG_V, LOG_V, LOG_V, LOG_V, LOG_V, LOG_V, LOG_V, LOG_V, LOG_3, LOG_2)(__VA_ARGS__)

#ifndef LOG_2
#	define LOG_2(level, msg) LOG_PRINTLN(msg);
#endif
#ifndef LOG_3
#	define LOG_3(level, tag, msg) LOG_PRINTLN("[" tag "] " msg);
#endif
#ifndef LOG_V
#	define LOG_V(level, tag, format, ...) LOG_PRINTF("[" tag "] " format "\n", __VA_ARGS__);
#endif

#ifndef LOG_ERROR
#	define LOG_ERROR(tag, ...) if (CHECK_LOG_LEVEL(tag, LOG_LEVEL_ERROR)) _LOG_IMPL(LOG_LEVEL_ERROR, tag, __VA_ARGS__)
#endif
#ifndef LOG_WARN
#	define LOG_WARN(tag, ...)  if (CHECK_LOG_LEVEL(tag, LOG_LEVEL_WARN))  _LOG_IMPL(LOG_LEVEL_WARN, tag, __VA_ARGS__)
#endif
#ifndef LOG_INFO
#	define LOG_INFO(tag, ...)  if (CHECK_LOG_LEVEL(tag, LOG_LEVEL_INFO))  _LOG_IMPL(LOG_LEVEL_INFO, tag, __VA_ARGS__)
#endif
#ifndef LOG_DEBUG
#	define LOG_DEBUG(tag, ...) if (CHECK_LOG_LEVEL(tag, LOG_LEVEL_DEBUG)) _LOG_IMPL(LOG_LEVEL_DEBUG, tag, __VA_ARGS__)
#endif
#ifndef LOG_TRACE
#	define LOG_TRACE(tag, ...) if (CHECK_LOG_LEVEL(tag, LOG_LEVEL_TRACE)) _LOG_IMPL(LOG_LEVEL_TRACE, tag, __VA_ARGS__)
#endif

////////////////////////////////////////////////////////////////////////////////
// Extra utils

#define WHERE_AM_I_STR __FILE__ ":" STRINGIFY(__LINE__)

#define STRINGIFY_DETAIL(x) #x
#define STRINGIFY(x) STRINGIFY_DETAIL(x)
