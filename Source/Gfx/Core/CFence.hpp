#ifndef WDK_CFENCE_HPP
#define WDK_CFENCE_HPP

#include "WdkGfx.hpp"

struct ID3D12Fence;

class CFence : public IFence
{
private:
	ID3D12Fence* m_pID3D12Fence;
	HANDLE       m_hFenceEvent;

private:
	CFence(VOID);
	~CFence(VOID);

	BOOL Initialize(ID3D12Fence* pInterface);
	VOID Uninitialize(VOID);

public:
	static CFence* CreateInstance(ID3D12Fence* pInterface);
	static VOID    DestroyInstance(CFence* pFence);
};

#endif //WDK_CFENCE_HPP
