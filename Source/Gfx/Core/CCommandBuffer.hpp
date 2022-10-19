#ifndef WDK_CCOMMAND_BUFFER_HPP
#define WDK_CCOMMAND_BUFFER_HPP

#include "WdkGfx.hpp"

struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;

class CCommandBuffer : public ICommandBuffer
{
private:
	BOOL                       m_bWriteStatus;
	ID3D12CommandAllocator*    m_pID3D12CommandAllocator;
	ID3D12GraphicsCommandList* m_pID3D12GraphicsCommandList;

public:
	CCommandBuffer(VOID);
	~CCommandBuffer(VOID);

	BOOL Initialize(ID3D12CommandAllocator* pICommandAllocator, ID3D12GraphicsCommandList* pIGraphicsCommandList);
	VOID Uninitialize(VOID);

	virtual VOID ClearRenderBuffer(UINT64 CpuDescriptor, FLOAT RGBA[]);
	virtual VOID Present(HANDLE hResource);
	virtual VOID Render(IMesh* pIMesh);
	virtual VOID SetViewport(UINT x, UINT y, UINT w, UINT h, FLOAT min_depth, FLOAT max_depth);
	virtual VOID SetRenderer(IRenderer* pIRenderer);
	virtual VOID SetRenderTarget(HANDLE hResource, UINT64 CpuDescriptor);

	virtual BOOL Finalize(VOID);
	virtual BOOL Reset(VOID);

	virtual ID3D12GraphicsCommandList* GetD3D12GraphicsCommandList(VOID);
};

#endif // WDK_CCOMMAND_BUFFER_HPP
