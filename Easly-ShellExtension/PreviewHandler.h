// PreviewHandler.h : Declaration of the preview handler

#pragma once
#include "resource.h"       // main symbols

#define AFX_PREVIEW_STANDALONE
#include <atlhandler.h>
#include <atlhandlerimpl.h>
#include "Easly.h"
#include <atlpreviewctrlimpl.h>
#include "Shlobj.h"

#include "EaslyShellExtension_i.h"

using namespace ATL;

class CPreviewHandler;

// CPreviewCtrl implementation
class CPreviewCtrl : public CAtlPreviewCtrlImpl
{
protected:
	virtual void DoPaint(HDC hdc)
	{
		DWORD dwType;
		WCHAR wszPath[1024] = { 0 };
		DWORD cbData = 1024;

		// Gets the path to this extension.
		RegGetValue(HKEY_CLASSES_ROOT, L"CLSID\\{8cdccead-b092-409a-af3b-46dc868b5e7f}\\InprocServer32", NULL, RRF_RT_ANY, &dwType, wszPath, &cbData);

		// Replace the extension file name with the preview file name.
		PWSTR pwsz = wszPath + wcslen(wszPath) - 1;
		while (pwsz > wszPath && *pwsz != L'\\')
			pwsz--;
		wcscpy(pwsz, L"\\Easly-Preview.exe");

		// Obtain the path to a temporary file. Avoid GetTempPath it gives a path with access denied (WTF Mikeysoft).
		WCHAR wszTempPath[1024] = { 0 };

		PWSTR pwszLocalLow = NULL;
		SHGetKnownFolderPath(FOLDERID_LocalAppDataLow, 0, 0, &pwszLocalLow);
		if (pwszLocalLow != NULL)
		{
			wcscpy(wszTempPath, pwszLocalLow);
			wcscat(wszTempPath, L"\\Temp");
			CreateDirectory(wszTempPath, NULL);
		}
		else
			GetTempPath(1024, wszTempPath);

		WCHAR wszDestinationFileName[1024] = { 0 };
		GetTempFileName(wszTempPath, L"easly", 0, wszDestinationFileName);

		// Get the source file when it was saved.
		Easly *pDocument = (Easly *)m_pDocument;
		CString cSourceFileName = pDocument->m_FilePath;

		WCHAR wszCommandLine[1024] = { 0 };
		wsprintf(wszCommandLine, L"%s %s %s", wszPath, cSourceFileName, wszDestinationFileName);

		// Run the process that creates a bitmap.
		STARTUPINFO StartupInfo = { sizeof(StartupInfo) };
		PROCESS_INFORMATION ProcInfo = { 0 };
		CreateProcess(wszPath, wszCommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &ProcInfo);
		DWORD dwError = WaitForSingleObject(ProcInfo.hProcess, 20000);

		// Load and display the bitmap.
		HBITMAP hBitmap = (HBITMAP)LoadImage(0, wszDestinationFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
		BITMAP bitmap;
		GetObject(hBitmap, sizeof(bitmap), &bitmap);
		BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, oldBitmap);
		DeleteDC(hdcMem);
		DeleteObject(hBitmap);

		//DeleteFile(cSourceFileName);
		DeleteFile(wszDestinationFileName);

		/*
		wsprintf(wszCommandLine, L"* %d *", pDocument->m_Debug);
		CString strData = wszCommandLine;
		TextOut(hdc, 10, 20, strData, strData.GetLength());
		*/
	}
};

// CPreviewHandler

class ATL_NO_VTABLE CPreviewHandler :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPreviewHandler, &CLSID_Preview>,
	public CPreviewHandlerImpl <CPreviewHandler>
{
public:
	CPreviewHandler()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_PREVIEW_HANDLER)
DECLARE_NOT_AGGREGATABLE(CPreviewHandler)

BEGIN_COM_MAP(CPreviewHandler)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY(IOleWindow)
	COM_INTERFACE_ENTRY(IInitializeWithStream)
	COM_INTERFACE_ENTRY(IPreviewHandler)
	COM_INTERFACE_ENTRY(IPreviewHandlerVisuals)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
		CPreviewHandlerImpl<CPreviewHandler>::FinalRelease();
	}

protected:
	virtual IPreviewCtrl* CreatePreviewControl()
	{
		// This class is defined at the beginning of this header
		CPreviewCtrl *pPreviewCtrl = nullptr;
		ATLTRY(pPreviewCtrl = new CPreviewCtrl());
		return pPreviewCtrl;
	}

	virtual IDocument* CreateDocument()
	{
		Easly *pDocument = nullptr;
		ATLTRY(pDocument = new Easly());
		return pDocument;
	}
};

OBJECT_ENTRY_AUTO(__uuidof(Preview), CPreviewHandler)
