#include "CFile.hpp"

#include <strsafe.h>
#include <windows.h>

IFile* IFile::Open(PCWCHAR Path)
{
	return static_cast<IFile*>(CFile::Open(Path));
}

IFile* IFile::Open(const FILE_PATH& Path)
{
	BOOL Status = TRUE;
	IFile* pIFile = NULL;
	WCHAR Buffer[1024] = {0};

	if (StringCchCatEx(Buffer, _countof(Buffer), Path.Directory, NULL, NULL, 0) == S_OK)
	{
		Status = (StringCchCatEx(Buffer, _countof(Buffer), Path.FileName, NULL, NULL, 0) == S_OK) ? TRUE : FALSE;
	}
	else
	{
		Status = FALSE;
	}

	if (Status == TRUE)
	{
		pIFile = IFile::Open(Buffer);
	}

	return pIFile;
}

VOID IFile::Close(IFile* pIFile)
{
	return CFile::Close(static_cast<CFile*>(pIFile));
}

CFile::CFile()
{
	hFile = NULL;
}

CFile::~CFile()
{
	Close(this);
}

CFile* CFile::Open(PCWCHAR Path)
{
	CFile* pCFile = new CFile();

	pCFile->hFile = CreateFile(Path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (pCFile->hFile == INVALID_HANDLE_VALUE)
	{
		delete pCFile;
		pCFile = NULL;

		Console::Write(L"Error: Could not open file %s for reading\n", Path);
	}

	return pCFile;
}

VOID CFile::Close(CFile* pCFile)
{
	if ((pCFile->hFile != 0) && (pCFile->hFile != INVALID_HANDLE_VALUE))
	{
		CloseHandle(pCFile->hFile);
		pCFile->hFile = NULL;
	}
}

BOOL CFile::Read(BYTE** ppBuffer, DWORD* pSize)
{
	BOOL Status = TRUE;
	LARGE_INTEGER lpFileSize = {};

	if ((ppBuffer == NULL) || (pSize == NULL))
	{
		Status = FALSE;
		Console::Write(L"Error: Invalid or null buffer/size parameters\n");
	}

	if (Status == TRUE)
	{
		if (GetFileSizeEx(hFile, &lpFileSize) != TRUE)
		{
			Status = FALSE;
			Console::Write(L"Error: Could not get file size\n");
		}
	}

	if (Status == TRUE)
	{
		*ppBuffer = reinterpret_cast<BYTE*>(Memory::Allocate(lpFileSize.QuadPart, TRUE));
		if (*ppBuffer == NULL)
		{
			Status = FALSE;
			Console::Write(L"Error: Could not allocate buffer to read file\n");
		}
	}

	if (Status == TRUE)
	{
		if (ReadFile(hFile, *ppBuffer, static_cast<DWORD>(lpFileSize.QuadPart), pSize, NULL) != TRUE)
		{
			Status = FALSE;
			Console::Write(L"Error: Could read file contents\n");
		}
	}

	return Status;
}
