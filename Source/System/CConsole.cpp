#include "Wdk.hpp"
#include "CConsole.hpp"

#include <cstdarg>

#include <windows.h>
#include <strsafe.h>

HANDLE CConsole::m_hStdOut = NULL;
CWSTR  CConsole::m_pBuffer = NULL;

BOOL Console::Write(CONST_CWSTR Msg, ...)
{
	BOOL Status = TRUE;

	VA_LIST Args;
	va_start(Args, Msg);

	Status = CConsole::Write(Msg, Args);

	va_end(Args);

	return Status;
}

BOOL Console::Write(CONST_CWSTR Msg, VA_LIST Args)
{
	BOOL Status = TRUE;

	Status = CConsole::Write(Msg, Args);

	return Status;
}

BOOL CConsole::Initialize(VOID)
{
	BOOL Status = TRUE;

	m_hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	if ((m_hStdOut == NULL) || (m_hStdOut == INVALID_HANDLE_VALUE))
	{
		Status = FALSE;
	}

	if (Status == TRUE)
	{
		m_pBuffer = reinterpret_cast<CWSTR>(Memory::Allocate(MaxLength * sizeof(WCHAR), TRUE));
	}

	return Status;
}

BOOL CConsole::Uninitialize(VOID)
{
	BOOL Status = TRUE;

	m_hStdOut = NULL;

	if (m_pBuffer != NULL)
	{
		Memory::Release(m_pBuffer);
		m_pBuffer = NULL;
	}

	return Status;
}

BOOL CConsole::Write(CONST_CWSTR Msg, VA_LIST Args)
{
	BOOL Status = TRUE;
	SIZE_T CharsFree = 0;
	DWORD  CharsUsed = 0;
	DWORD  CharsWritten = 0;

	if (StringCchVPrintfEx(m_pBuffer, MaxLength, NULL, &CharsFree, 0, Msg, Args) == S_OK)
	{
		CharsUsed = static_cast<DWORD>(MaxLength - CharsFree);
	}
	else
	{
		Status = FALSE;
	}

	if (Status == TRUE)
	{
		Status = WriteConsole(m_hStdOut, m_pBuffer, CharsUsed, &CharsWritten, NULL);
	}

	if (Status == TRUE)
	{
		Status = (CharsWritten == CharsUsed) ? TRUE : FALSE;
	}

	return Status;
}
