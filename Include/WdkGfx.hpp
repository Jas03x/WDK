#ifndef WDK_GFX__HPP
#define WDK_GFX__HPP

#include <WdkDef.hpp>

class IWindow;

// IRenderer
struct SHADER_BYTECODE
{
	BYTE* pCode;
	DWORD Size;
};

enum INPUT_ELEMENT
{
	INPUT_ELEMENT_INVALID = 0,
	INPUT_ELEMENT_POSITION = 1,
	INPUT_ELEMENT_COLOR = 2
};

enum INPUT_ELEMENT_FORMAT
{
	INPUT_ELEMENT_FORMAT_INVALID = 0,
	INPUT_ELEMENT_FORMAT_XYZ_8F = 1, INPUT_ELEMENT_FORMAT_RGB_8F = 1,
	INPUT_ELEMENT_FORMAT_XYZ_16F = 2, INPUT_ELEMENT_FORMAT_RGB_16F = 2,
	INPUT_ELEMENT_FORMAT_XYZ_32F = 3, INPUT_ELEMENT_FORMAT_RGB_32F = 3,
	INPUT_ELEMENT_FORMAT_XYZW_8F = 4, INPUT_ELEMENT_FORMAT_RGBA_8F = 4,
	INPUT_ELEMENT_FORMAT_XYZW_16F = 5, INPUT_ELEMENT_FORMAT_RGBA_16F = 5,
	INPUT_ELEMENT_FORMAT_XYZW_32F = 6, INPUT_ELEMENT_FORMAT_RGBA_32F = 6,
};

enum INPUT_ELEMENT_TYPE
{
	INPUT_ELEMENT_TYPE_INVALID = 0,
	INPUT_ELEMENT_TYPE_PER_VERTEX = 1,
	INPUT_ELEMENT_TYPE_PER_INSTANCE = 2
};

struct INPUT_ELEMENT_DESC
{
	INPUT_ELEMENT        Element;
	UINT                 ElementIndex;
	INPUT_ELEMENT_FORMAT ElementFormat;
	UINT                 InputSlot;
	UINT                 AlignedByteOffset;
	INPUT_ELEMENT_TYPE   Type;
	UINT                 InstanceStep;
};

struct INPUT_BUFFER_LAYOUT
{
	INPUT_ELEMENT_DESC* pInputElements;
	UINT                NumInputs;
};

struct RENDERER_DESC
{
	SHADER_BYTECODE     VertexShader;
	SHADER_BYTECODE     PixelShader;
	INPUT_BUFFER_LAYOUT InputLayout;
};

class __declspec(novtable) IRenderer
{
public:
};

BOOL ReadShaderBytecode(const FILE_PATH& Path, SHADER_BYTECODE& rDesc);
VOID ReleaseShaderBytecode(SHADER_BYTECODE& rDesc);

// ICommandList
class __declspec(novtable) ICommandList
{

};

// IFence
class __declspec(novtable) IFence
{

};

// IGfxDevice
class __declspec(novtable) IGfxDevice
{
public:
	struct Desc
	{
		IWindow* pIWindow;

		UINT64 UploadHeapSize;
		UINT64 PrimaryHeapSize;
	};

	virtual IRenderer*    CreateRenderer(const RENDERER_DESC& rDesc) = 0;
	virtual VOID          DestroyRenderer(IRenderer* pIRenderer) = 0;

	virtual ICommandList* CreateCommandList(VOID) = 0;
	virtual VOID          DestroyCommandList(ICommandList* pICommandList) = 0;

	virtual IFence*       CreateFence(VOID) = 0;
	virtual VOID          DestroyFence(IFence* pIFence) = 0;

public:
	static IGfxDevice* CreateInstance(Desc& rDesc);
	static VOID        DestroyInstance(IGfxDevice* pIDevice);
};

#endif // WDK_GFX__HPP
