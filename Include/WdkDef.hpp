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
typedef unsigned char       UCHAR;
typedef wchar_t				WCHAR;
typedef short				SHORT;
typedef unsigned short      USHORT;
typedef int					INT;
typedef unsigned int        UINT;
typedef long				LONG;
typedef unsigned long		ULONG;
typedef long long           LONGLONG;
typedef unsigned long long  ULONGLONG;
typedef unsigned __int64	SIZE_T;

typedef signed __int8       INT8;
typedef signed __int16      INT16;
typedef signed __int32      INT32;
typedef signed __int64      INT64;
typedef unsigned __int8     UINT8;
typedef unsigned __int16    UINT16;
typedef unsigned __int32    UINT32;
typedef unsigned __int64    UINT64;

typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long		DWORD;
typedef unsigned long long  QWORD;

typedef float				FLOAT;
typedef double              DOUBLE;

typedef char                CHAR;
typedef wchar_t             WCHAR;

typedef char*               CSTR;
typedef wchar_t*            CWSTR;

typedef const char*         CONST_CSTR;
typedef const wchar_t*      CONST_CWSTR;

typedef void*               HANDLE;

typedef void*				PVOID;

typedef char*				VA_LIST;

enum STATUS
{
	SUCCESS         = 0x00000000L,
	UNSUCCESSFUL    = 0xC0000001L,
	NOT_IMPLEMENTED = 0xC0000002L
};

typedef struct FILE_PATH
{
	CONST_CWSTR Directory;
	CONST_CWSTR FileName;

	inline FILE_PATH(CONST_CWSTR directory, CONST_CWSTR filename)
	{
		Directory = directory;
		FileName  = filename;
	}
} FILE_PATH;

enum : UINT
{
	KB = 1024 * sizeof(BYTE),
	MB = 1024 * KB,
	GB = 1024 * MB
};

#endif // WDK_DEF__HPP