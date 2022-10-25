#ifndef WDK_CCOMMAND_BUFFER_HPP
#define WDK_CCOMMAND_BUFFER_HPP

#include "WdkGfx.hpp"

struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;

class CCommandBuffer : public ICommandBuffer
{
private:
	enum STATE
	{
		STATE_ERROR     = 0,
		STATE_RESET     = 1,
		STATE_RECORDING = 2,
		STATE_CLOSED    = 3
	};

	COMMAND_BUFFER_TYPE        m_Type;

	STATE                      m_State;

	ID3D12CommandAllocator*    m_pID3D12CommandAllocator;
	ID3D12GraphicsCommandList* m_pID3D12CommandList;

public:
	CCommandBuffer(VOID);
	~CCommandBuffer(VOID);

	BOOL                       Initialize(COMMAND_BUFFER_TYPE Type, ID3D12CommandAllocator* pICommandAllocator, ID3D12GraphicsCommandList* pICommandList);
	VOID                       Uninitialize(VOID);

	virtual VOID               ClearRenderBuffer(const RenderBuffer& rBuffer, CONST FLOAT RGBA[]);
	virtual VOID               Present(const RenderBuffer& rBuffer);
	virtual VOID               Render(IMesh* pIMesh);
	virtual VOID               SetViewport(UINT x, UINT y, UINT w, UINT h, FLOAT min_depth, FLOAT max_depth);
	virtual VOID               SetRenderer(IRenderer* pIRenderer);
	virtual VOID               SetRenderTarget(const RenderBuffer& rBuffer);

	virtual BOOL               Finalize(VOID);
	virtual BOOL               Reset(VOID);

	COMMAND_BUFFER_TYPE        GetType(VOID);
	ID3D12GraphicsCommandList* GetD3D12CommandList(VOID);
};

#endif // WDK_CCOMMAND_BUFFER_HPP
