#ifndef WDK_CFILE_HPP
#define WDK_CFILE_HPP

#include "Wdk.hpp"
#include "WdkDef.hpp"

class CFile : public File
{
private:
	HANDLE hFile;

private:
	CFile();
	~CFile();

public:
	static CFile* Open(PCWCHAR Path);
	static VOID Close(CFile* pCFile);

public:
	virtual BOOL Read(BYTE** ppBuffer, DWORD* pSize);
};

#endif // WDK_CFILE_HPP