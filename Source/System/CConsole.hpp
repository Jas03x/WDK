#ifndef WDK_CCONSOLE_HPP
#define WDK_CCONSOLE_HPP

#include "WdkDef.hpp"

class CConsole
{
private:
	enum { MaxLength = 1024 };

	HANDLE m_hStdOut;
	PWCHAR m_pBuffer;

public:
	CConsole(VOID);
	~CConsole(VOID);

	BOOL Initialize(VOID);
	VOID Uninitialize(VOID);

	BOOL Write(PCWCHAR Msg, va_list Args);
};

#endif // WDK_CCONSOLE_HPP