#include "Wdk.hpp"
#include "CConsole.hpp"

#include <cstdarg>

#include <windows.h>
#include <strsafe.h>

HANDLE CConsole::m_hStdOut = NULL;
PWCHAR CConsole::m_pBuffer = NULL;

BOOL Console::Write(PCWCHAR Msg, ...)
{
	BOOL Status = TRUE;

	va_list Args;
	va_start(Args, Msg);

	Status = CConsole::Write(Msg, Args);

	va_end(Args);

	return Status;
}

BOOL Console::Write(PCWCHAR Msg, va_list Args)
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
		m_pBuffer = reinterpret_cast<PWCHAR>(Memory::Allocate(MaxLength * sizeof(WCHAR), TRUE));
	}

	return Status;
}

VOID CConsole::Uninitialize(VOID)
{
	m_hStdOut = NULL;

	if (m_pBuffer != NULL)
	{
		Memory::Release(m_pBuffer);
		m_pBuffer = NULL;
	}
}

BOOL CConsole::Write(PCWCHAR Msg, va_list Args)
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
