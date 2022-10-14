#ifndef WDK_CCOMMAND_QUEUE_HPP
#define WDK_CCOMMAND_QUEUE_HPP

#include "WdkDef.hpp"
#include "WdkGfx.hpp"

struct ID3D12CommandQueue;
struct ID3D12CommandAllocator;
struct ID3D12Fence;

class CCommandQueue : public ICommandQueue
{
private:
	enum : UINT { COMMAND_QUEUE_TIMEOUT = 1000 };

	ID3D12CommandQueue*     m_pID3D12CommandQueue;
	ID3D12CommandAllocator* m_pID3D12CommandAllocator;
	ID3D12Fence*            m_pID3D12Fence;

	HANDLE                  m_hFenceEvent;
	UINT64                  m_FenceValue;

public:
	CCommandQueue(VOID);
	~CCommandQueue(VOID);

	BOOL Initialize(ID3D12CommandQueue* pID3D12CommandQueue, ID3D12CommandAllocator* pID3D12CommandAllocator, ID3D12Fence* pID3D12Fence);
	VOID Uninitialize(VOID);

	BOOL                    Wait(VOID);

	ID3D12CommandQueue*     GetD3D12CommandQueue(VOID);
	ID3D12CommandAllocator* GetD3D12CommandAllocator(VOID);
};

#endif // WDK_CCOMMAND_QUEUE_HPP