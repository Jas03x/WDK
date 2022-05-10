#ifndef CCONSOLE_HPP
#define CCONSOLE_HPP

#include "Defines.hpp"

class CConsole
{
private:
	enum { MaxLength = 1024 };

	HANDLE m_hStdOut;
	PWCHAR m_pBuffer;

public:
	CConsole();
	~CConsole();

	BOOL Initialize(VOID);
	VOID Uninitialize(VOID);

	BOOL Write(LPCWSTR Msg, va_list Args);
};

#endif // CCONSOLE_HPP