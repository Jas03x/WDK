#ifndef WDK_DEF__HPP
#define WDK_DEF__HPP

#define CONST	const
#define VOID	void

#ifndef NULL
#define NULL	0
#endif

#define TRUE	1
#define FALSE   0

typedef signed char			int8_t;
typedef signed short		int16_t;
typedef signed int			int32_t;
typedef signed long long	int64_t;
typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;
typedef unsigned long long	uint64_t;

typedef int					BOOL;
typedef char				CHAR;
typedef wchar_t				WCHAR;
typedef short				SHORT;
typedef int					INT;
typedef unsigned int        UINT;
typedef long				LONG;
typedef unsigned long		ULONG;
typedef long long           LONGLONG;
typedef unsigned long long  ULONGLONG;
typedef unsigned __int64	SIZE_T;

typedef unsigned __int64    UINT64;

typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long		DWORD;
typedef unsigned long long  QWORD;

typedef float				FLOAT;

typedef char				*PCH,  *PCHAR;
typedef wchar_t             *PWCH, *PWCHAR;

typedef const char			*PCCH, *PCCHAR;
typedef const wchar_t		*PCWCH, *PCWCHAR;

typedef unsigned __int64    WPARAM;
typedef __int64				LPARAM;

typedef unsigned short      ATOM;
typedef void*				HANDLE;
typedef struct HWND__*		HWND;
typedef struct HINSTANCE__* HMODULE;
typedef struct HINSTANCE__* HINSTANCE;
typedef void*				PVOID;

typedef struct FILE_PATH
{
	PCWCHAR Directory;
	PCWCHAR FileName;

	inline FILE_PATH(PCWCHAR directory, PCWCHAR filename)
	{
		Directory = directory;
		FileName = filename;
	}
} FILE_PATH;

typedef char*				va_list;

enum STATUS
{
	SUCCESS         = 0x00000000L,
	UNSUCCESSFUL    = 0xC0000001L,
	NOT_IMPLEMENTED = 0xC0000002L
};

#endif // WDK_DEF__HPP