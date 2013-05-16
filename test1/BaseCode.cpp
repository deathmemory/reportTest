#include "stdafx.h"
//#include "CallLolInterfaceCommon.h"
//extern  _CallLolInterface LolInterfaceEncAddr=0;


#include "BaseCode/hook/inline/HookInternal.h"
#include "BaseCode/File/CFileIO.hpp"
#include "BaseCode/Pairs/Pairs.h"
#ifdef _HOOK_INTERNAL
	#include "BaseCode/hook/inline/HookInternal.cpp"
#endif

#ifdef _FILEIO
	#include "BaseCode/File/CFileIO.cpp"
#endif

#ifdef _STRING_EX
	#include "BaseCode/String/STRING_EX.cpp"
#endif

#ifdef _SYSTEM_INTERFACE
	#include "BaseCode/System/System.cpp"
#endif

#ifdef _CRndNum
	#include "basecode/stdlib/cRandom.cpp"
#endif

#ifdef _PAIRS
	#include "BaseCode/Pairs/Pairs.cpp"
#endif
#ifdef _ROTATE
	#include "basecode/crypt/rotate.cpp
#endif