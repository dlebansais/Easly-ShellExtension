// Easly.h : Declaration of the Easly class

#pragma once

#include <atlhandlerimpl.h>

using namespace ATL;

class Easly : public CAtlDocumentImpl
{
public:
	Easly(void)
	{
	}

	virtual ~Easly(void)
	{
		if (m_FilePath.GetLength() > 0)
			DeleteFile(m_FilePath);
	}

	virtual HRESULT LoadFromStream(IStream* pStream, DWORD grfMode);
	virtual void InitializeSearchContent();

	CString m_FilePath;

protected:
	void SetSearchContent(CString& value);
	virtual void OnDrawThumbnail(HDC hDrawDC, LPRECT lprcBounds);
};
