// MediaInfoShellExt.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "resource.h"
#include "MediaInfoShellExt.h"


class CMediaInfoShellExtModule : public CAtlDllModuleT< CMediaInfoShellExtModule >
{
public :
	DECLARE_LIBID(LIBID_MediaInfoShellExtLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_MEDIAINFOSHELLEXT, "{3E614C78-4868-4FAE-BA50-FE5D7AACF001}")
};

CMediaInfoShellExtModule _AtlModule;


#ifdef _MANAGED
#pragma managed(push, off)
#endif

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
    return _AtlModule.DllMain(dwReason, lpReserved); 
}

#ifdef _MANAGED
#pragma managed(pop)
#endif




// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}

