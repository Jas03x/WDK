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
	static CFile* Open(const wchar_t* Path);
	static void   Close(CFile* pCFile);

public:
	virtual bool  Read(byte** ppBuffer, uint32_t* pSize);
};

#endif // WDK_CFILE_HPP