#ifndef WDK_GFX__HPP
#define WDK_GFX__HPP

#include <WdkDef.hpp>

class IWindow;

CONST FLOAT MIN_DEPTH = 0.0f;
CONST FLOAT MAX_DEPTH = 1.0f;

// Shared structure between system and gfx
#ifndef WDK_RENDER_BUFFER
#define WDK_RENDER_BUFFER
struct RenderBuffer
{
	HANDLE hResource;
	UINT64 CpuDescriptor;
};
#endif // WDK_RENDER_BUFFER

// IRendererState
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

struct RENDERER_STATE_DESC
{
	SHADER_BYTECODE     VertexShader;
	SHADER_BYTECODE     PixelShader;
	INPUT_BUFFER_LAYOUT InputLayout;
};

class __declspec(novtable) IRendererState
{
public:
};

BOOL ReadShaderBytecode(CONST FILE_PATH& Path, SHADER_BYTECODE& rDesc);
VOID ReleaseShaderBytecode(SHADER_BYTECODE& rDesc);

// IConstantBuffer
struct CONSTANT_BUFFER_DESC
{
	UINT Size;
};

class __declspec(novtable) IConstantBuffer
{
public:
	virtual VOID* GetCpuVA(VOID) = 0;
};

// IMesh
struct MESH_DESC
{
	UINT BufferSize;
	UINT Stride;
	UINT NumVertices;
};

class __declspec(novtable) IMesh
{
public:
};

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
	virtual VOID ClearRenderBuffer(const RenderBuffer& rBuffer, CONST FLOAT RGBA[]) = 0;
	virtual VOID Present(const RenderBuffer& rBuffer) = 0;
	virtual VOID Render(IMesh* pIMesh) = 0;
	virtual VOID SetViewport(UINT x, UINT y, UINT w, UINT h, FLOAT min_depth, FLOAT max_depth) = 0;
	virtual VOID ProgramPipeline(IRendererState* pIRendererState) = 0;
	virtual VOID SetRenderTarget(const RenderBuffer& rBuffer) = 0;

	virtual BOOL Finalize(VOID) = 0;
	virtual BOOL Reset(VOID) = 0;
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

// IGfxDevice
class __declspec(novtable) IGfxDevice
{
public:
	virtual ICommandBuffer*  CreateCommandBuffer(COMMAND_BUFFER_TYPE Type) = 0;
	virtual VOID             DestroyCommandBuffer(ICommandBuffer* pICommandBuffer) = 0;

	virtual IRendererState*  CreateRendererState(CONST RENDERER_STATE_DESC& rDesc) = 0;
	virtual VOID             DestroyRendererState(IRendererState* pIRendererState) = 0;

	virtual IConstantBuffer* CreateConstantBuffer(CONST CONSTANT_BUFFER_DESC& rDesc) = 0;
	virtual VOID             DestroyConstantBuffer(IConstantBuffer* pIConstantBuffer) = 0;

	virtual IMesh*           CreateMesh(CONST VOID* pVertexData, MESH_DESC& rDesc) = 0;
	virtual VOID             DestroyMesh(IMesh* pIMesh) = 0;

	virtual BOOL             SubmitCommandBuffer(ICommandBuffer* pICommandBuffer) = 0;
	virtual BOOL             SyncQueue(COMMAND_QUEUE_TYPE Type) = 0;
};

// Device Factory
class __declspec(novtable) DeviceFactory
{
public:
	struct Descriptor
	{
		UINT64 UploadHeapSize;
		UINT64 PrimaryHeapSize;
		UINT64 ConstantBufferHeapSize;
	};

	static IGfxDevice* CreateInstance(IWindow* pIWindow, const Descriptor& rDesc);
	static VOID        DestroyInstance(IGfxDevice* pIDevice);
};

#endif // WDK_GFX__HPP
