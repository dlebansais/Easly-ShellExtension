// dllmain.h : Declaration of module class.

class CEaslyShellExtensionModule : public ATL::CAtlDllModuleT< CEaslyShellExtensionModule >
{
public :
	DECLARE_LIBID(LIBID_EaslyShellExtensionLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_EASLYSHELLEXTENSION, "{59988811-95c5-4c88-9437-390653ae2a72}")
};

extern class CEaslyShellExtensionModule _AtlModule;
