// Easly.cpp : Implementation of the Easly class

#include "stdafx.h"
#include <propkey.h>
#include "Easly.h"
#include "Shlobj.h"

// Copy the content of a stream to a file.
void CopyStreamToFile(IStream *pStream, PCWSTR pwszFileName)
{
	HANDLE hFile = CreateFile(pwszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);

	BYTE Buffer[4096];
	DWORD dwRead = 0;
	DWORD dwCount = sizeof(Buffer);
	DWORD dwWritten;

	LARGE_INTEGER zero = { 0 };
	pStream->Seek(zero, SEEK_SET, NULL);

	do
	{
		pStream->Read(Buffer, dwCount, &dwRead);
		WriteFile(hFile, Buffer, dwRead, &dwWritten, NULL);
	} 
	while (dwRead == dwCount);

	CloseHandle(hFile);
}

HRESULT Easly::LoadFromStream(IStream* pStream, DWORD grfMode)
{
	// Load from stream your document data
	UNREFERENCED_PARAMETER(grfMode);

	// Obtain the path to a temporary file. Avoid GetTempPath it gives a path with access denied (WTF Mikeysoft).
	WCHAR wszTempPath[1024] = { 0 };

	PWSTR pwszLocalLow = NULL;
	SHGetKnownFolderPath(FOLDERID_LocalAppDataLow, 0, 0, &pwszLocalLow);
	if (pwszLocalLow != NULL)
	{
		wcscpy(wszTempPath, pwszLocalLow);
		wcscat(wszTempPath, L"\\Temp");

		// Create the folder, it might not exist.
		CreateDirectory(wszTempPath, NULL);
	}
	else
		GetTempPath(1024, wszTempPath);

	WCHAR wszSourceFileName[1024] = { 0 };
	GetTempFileName(wszTempPath, L"easly", 0, wszSourceFileName);

	// Delete the file if it already exists.
	if (m_FilePath.GetLength() > 0)
		DeleteFile(m_FilePath);

	CopyStreamToFile(pStream, wszSourceFileName);
	m_FilePath = wszSourceFileName;

	return S_OK;
}

void Easly::InitializeSearchContent()
{
	// initialise search content from document's data as the following value
	CString value = _T("test;content;");
	SetSearchContent(value);
}

void Easly::SetSearchContent(CString& value)
{
	// Assigns search content to PKEY_Search_Contents key
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

void Easly::OnDrawThumbnail(HDC hDrawDC, LPRECT lprcBounds)
{
	HBRUSH hDrawBrush = CreateSolidBrush(RGB(255, 255, 255));
	FillRect(hDrawDC, lprcBounds, hDrawBrush);

	HFONT hStockFont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
	LOGFONT lf;

	GetObject(hStockFont, sizeof(LOGFONT), &lf);
	lf.lfHeight = 34;

	HFONT hDrawFont = CreateFontIndirect(&lf);
	HFONT hOldFont = (HFONT) SelectObject(hDrawDC, hDrawFont);

	CString strText = _T("TODO: implement thumbnail drawing here");
	DrawText(hDrawDC, strText, strText.GetLength(), lprcBounds, DT_CENTER | DT_WORDBREAK);

	SelectObject(hDrawDC, hDrawFont);
	SelectObject(hDrawDC, hOldFont);

	DeleteObject(hDrawBrush);
	DeleteObject(hDrawFont);
}
