#ifndef _XBOX_CONTROLLER_H_
#define _XBOX_CONTROLLER_H_

#include "DefinicoesImportantes.h"

#ifdef _MSC_VER

// No MFC
#define WIN32_LEAN_AND_MEAN

// We need the Windows Header and the XInput Header
#include <windows.h>
#include <XInput.h>
//#include <VersionHelpers.h>

// Now, the XInput Library
// NOTE: COMMENT THIS OUT IF YOU ARE NOT USING A COMPILER THAT SUPPORTS THIS METHOD OF LINKING LIBRARIES
#ifdef WIN_10
	#pragma comment(lib, "XInput.lib") //Windows 8+
#endif

#ifdef WIN_8
	#pragma comment(lib, "XInput.lib") //Windows 8+
#endif

#ifdef WIN_7
	#pragma comment(lib, "XInput9_1_0.lib") //Windows 7
#endif


// XBOX Controller Class Definition
class CXBOXController
{
private:
	XINPUT_STATE _controllerState;
	int _controllerNum;
public:
	CXBOXController(int playerNumber);
	XINPUT_STATE GetState();
	bool IsConnected();
	void Vibrate(int leftVal = 0, int rightVal = 0);
};
#endif

#endif
