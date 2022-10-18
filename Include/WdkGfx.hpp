#ifndef WDK_GFX__HPP
#define WDK_GFX__HPP

#include <WdkDef.hpp>

class IWindow;

const FLOAT MIN_DEPTH = 0.0f;
const FLOAT MAX_DEPTH = 1.0f;

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
	INPUT_ELEMENT_FORMAT_INVALID  = 0,
	INPUT_ELEMENT_FORMAT_XYZ_8F   = 1, INPUT_ELEMENT_FORMAT_RGB_8F   = 1,
	INPUT_ELEMENT_FORMAT_XYZ_16F  = 2, INPUT_ELEMENT_FORMAT_RGB_16F  = 2,
	INPUT_ELEMENT_FORMAT_XYZ_32F  = 3, INPUT_ELEMENT_FORMAT_RGB_32F  = 3,
	INPUT_ELEMENT_FORMAT_XYZW_8F  = 4, INPUT_ELEMENT_FORMAT_RGBA_8F  = 4,
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

// ICommandBuffer
enum COMMAND_BUFFER_TYPE
{
	COMMAND_BUFFER_TYPE_INVALID = 0,
	COMMAND_BUFFER_TYPE_DIRECT = 1,
	COMMAND_BUFFER_TYPE_GRAPHICS = 1,
	COMMAND_BUFFER_TYPE_COMPUTE = 2,
	COMMAND_BUFFER_TYPE_COPY = 3
};

class __declspec(novtable) ICommandBuffer
{
public:
	virtual BOOL Reset(VOID) = 0;
	virtual BOOL SetViewport(UINT x, UINT y, UINT w, UINT h, FLOAT min_depth, FLOAT max_depth) = 0;
	virtual BOOL SetRenderer(IRenderer* pIRenderer) = 0;
};

// class IQueue
enum COMMAND_QUEUE_TYPE
{
	COMMAND_QUEUE_TYPE_INVALID = 0,
	COMMAND_QUEUE_TYPE_DIRECT = 1,
	COMMAND_QUEUE_TYPE_GRAPHICS = 1,
	COMMAND_QUEUE_TYPE_COMPUTE = 2,
	COMMAND_QUEUE_TYPE_COPY = 3
};

class __declspec(novtable) ICommandQueue
{
public:
};

// IMesh
class __declspec(novtable) IMesh
{
public:
};

// IGfxDevice
class __declspec(novtable) IGfxDevice
{
public:
	virtual ICommandBuffer* CreateCommandBuffer(COMMAND_BUFFER_TYPE Type) = 0;
	virtual VOID            DestroyCommandBuffer(ICommandBuffer* pICommandBuffer) = 0;

	virtual IRenderer*      CreateRenderer(const RENDERER_DESC& rDesc) = 0;
	virtual VOID            DestroyRenderer(IRenderer* pIRenderer) = 0;

	virtual IMesh*          CreateMesh(CONST VOID* pVertexData, UINT SizeInBytes, UINT StrideInBytes) = 0;
	virtual VOID            DestroyMesh(IMesh* pIMesh) = 0;
};

// Device Factory
class __declspec(novtable) DeviceFactory
{
public:
	struct Descriptor
	{
		UINT64 UploadHeapSize;
		UINT64 PrimaryHeapSize;
	};

	static IGfxDevice* CreateInstance(IWindow* pIWindow, Descriptor& rDesc);
	static VOID        DestroyInstance(IGfxDevice* pIDevice);
};

#endif // WDK_GFX__HPP
