#ifndef WDK_CCOMMAND_BUFFER_HPP
#define WDK_CCOMMAND_BUFFER_HPP

#include "WdkGfx.hpp"

struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;

class CCommandBuffer : public ICommandBuffer
{
private:
	BOOL                       m_bIsFirstSubmission;

	ID3D12CommandAllocator*    m_pID3D12CommandAllocator;
	ID3D12GraphicsCommandList* m_pID3D12GraphicsCommandList;

public:
	CCommandBuffer(VOID);
	~CCommandBuffer(VOID);

	BOOL Initialize(ID3D12CommandAllocator* pICommandAllocator, ID3D12GraphicsCommandList* pIGraphicsCommandList);
	VOID Uninitialize(VOID);

	virtual BOOL Reset(VOID);
	virtual BOOL SetViewport(UINT x, UINT y, UINT w, UINT h, FLOAT min_depth, FLOAT max_depth);
	virtual BOOL SetRenderer(IRenderer* pIRenderer);

	virtual ID3D12GraphicsCommandList* GetD3D12GraphicsCommandList(VOID);
};

#endif // WDK_CCOMMAND_BUFFER_HPP
