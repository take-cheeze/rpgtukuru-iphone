#ifndef _INC__RPG2K__CONFIG_HPP_
#define _INC__RPG2K__CONFIG_HPP_

// environment things
#if defined(__GNUC__) && ( defined(__APPLE_CPP__) || defined(__APPLE_CC__) )
	#include <TargetConditionals.h>

	#define RPG2K_IS_MAC_OS_X (defined(TARGET_OS_MAC) && TARGET_OS_MAC)
	#define RPG2K_IS_IPHONE (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE)
	#define RPG2K_IS_IPHONE_SIMULATOR (defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR)

	#if (!defined(RPG2K_IS_BIG_ENDIAN) && !defined(RPG2K_IS_LITTLE_ENDIAN))
		#if defined(TARGET_RT_LITTLE_ENDIAN)
			#define RPG2K_IS_LITTLE_ENDIAN TARGET_RT_LITTLE_ENDIAN
		#endif
		#if defined(TARGET_RT_BIG_ENDIAN)
			#define RPG2K_IS_BIG_ENDIAN TARGET_RT_BIG_ENDIAN
		#endif
	#endif
#else
	#define RPG2K_IS_MAC_OS_X 0
	#define RPG2K_IS_IPHONE 0
	#define RPG2K_IS_IPHONE_SIMULATOR 0
#endif
#define RPG2K_IS_PSP defined(PSP)
#define RPG2K_IS_WINDOWS ( \
		defined(WIN32) || defined(_WIN32) || \
		defined(WIN64) || defined(_WIN64) \
	)
#define RPG2K_IS_LINUX defined(__linux)
#define RPG2K_IS_UNIX  defined(__unix )

// compiler things
#define RPG2K_IS_GCC defined(__GNUC__)
#define RPG2K_IS_CLANG defined(__clang__)
#if (RPG2K_IS_GCC || RPG2K_IS_CLANG)
	#define RPG2K_USE_RTTI defined(__GXX_RTTI)

	#if ( \
		!defined(RPG2K_IS_BIG_ENDIAN) && !defined(RPG2K_IS_LITTLE_ENDIAN) && \
		( defined(__BIG_ENDIAN__) || defined(__LITTLE_ENDIAN__) ) \
	)
		#define RPG2K_IS_BIG_ENDIAN defined(__BIG_ENDIAN__)
		#define RPG2K_IS_LITTLE_ENDIAN defined(__LITTLE_ENDIAN__)
	#endif
#endif

#define RPG2K_IS_MSVC defined(_MSC_VER)
#if RPG2K_IS_MSVC
	#default RPG2K_USE_RTTI defined(_CPPRTTI)

	#if (!defined(RPG2K_IS_BIG_ENDIAN) && !defined(RPG2K_IS_LITTLE_ENDIAN))
		/*
		 * windows only runs on little endian environment
		 */
		#define RPG2K_IS_LITTLE_ENDIAN 1
		#define RPG2K_IS_BIG_ENDIAN 0
	#endif
#endif


// default values
/*
 * by default RTTI is enabled
 */
#if !defined(RPG2K_USE_RTTI)
	#define RPG2K_USE_RTTI 1
#endif
/*
 * default endian is little endian
 */
#if (!defined(RPG2K_IS_BIG_ENDIAN) && !defined(RPG2K_IS_LITTLE_ENDIAN))
	#define RPG2K_IS_LITTLE_ENDIAN 1
	#define RPG2K_IS_BIG_ENDIAN 0
#endif

/*
 * set rpg maker version
 */
#if !( defined(RPG2000) || defined(RPG2000_VALUE) || defined(RPG2003) )
	// #define RPG2000
	#define RPG2000_VALUE
	// #define RPG2003
#endif

#if RPG2K_DEBUG
	/*
	 * set analyze at rpg2k::structure::Element deconstructor
	 */
	#define RPG2K_ANALYZE_AT_DESTRUCTOR 1
	/*
	 * only analyze non defined rpg2k::structure::Element
	 * ( checking with rpg2k::structure::Element::isDefined() )
	 */
	#define RPG2K_ONLY_ANALYZE_NON_DEFINED_ELEMENT 0
	/*
	 * call checkSerialize() at rpg2k::structure::Element constructor
	 * notice: only at constructor that has argumnt "Binary const&"
	 */
	#define RPG2K_CHECK_AT_CONSTRUCTOR 0
#else
	/*
	 * set analyze at rpg2k::structure::Element deconstructor
	 */
	#define RPG2K_ANALYZE_AT_DESTRUCTOR 0
	/*
	 * only analyze non defined rpg2k::structure::Element
	 * ( checking with rpg2k::structure::Element::isDefined() )
	 */
	#define RPG2K_ONLY_ANALYZE_NON_DEFINED_ELEMENT 0
	/*
	 * call checkSerialize() at rpg2k::structure::Element constructor
	 * notice: only at constructor that has argumnt "Binary const&"
	 */
	#define RPG2K_CHECK_AT_CONSTRUCTOR 0

	/*
	 * used at <cassert>
	 * disables assert
	 * NDEBUG -> "NO DEBUG"
	 */
	 #if !defined(NDEBUG)
		#define NDEBUG
	#endif
#endif

/*
 * set the allocator
 */
#define RPG2K_USE_DLMALLOC 0
#define RPG2K_USE_NEDMALLOC 1

#include <cassert>
#define rpg2k_assert(exp) assert(exp)

/*
 * Game Data Directory
 */
#if RPG2K_IS_IPHONE || RPG2K_IS_MAC_OS_X
	#define GAME_DATA_PATH "/User/Media/Photos/RPG2000/yoake"
	#define GAME_FIND_PATH "/User/Media/Photos/RPG2000/"
#elif RPG2K_IS_WINDOWS
	#define GAME_DATA_PATH "C:/Documents and Settings/gr/My Documents/My Dropbox/yoake"
	#define GAME_FIND_PATH "D:/ASCII/RPG2000/"
#else
	#define GAME_DATA_PATH "./yoake"
	#define GAME_FIND_PATH "./"
#endif

#endif // _INC__RPG2K__CONFIG_HPP_
