#ifndef WDK_CCONSOLE_HPP
#define WDK_CCONSOLE_HPP

#include "WdkDef.hpp"

class CConsole
{
private:
	enum { MaxLength = 1024 };

	static HANDLE m_hStdOut;
	static CWSTR  m_pBuffer;

public:
	static BOOL Initialize(VOID);
	static BOOL Uninitialize(VOID);

	static BOOL Write(CONST_CWSTR Msg, VA_LIST Args);
};

#endif // WDK_CCONSOLE_HPP