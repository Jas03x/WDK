#ifndef WDK_CCOMMAND_LIST_HPP
#define WDK_CCOMMAND_LIST_HPP

#include "WdkGfx.hpp"

struct ID3D12GraphicsCommandList;

class CCommandList : public ICommandList
{
private:
	ID3D12GraphicsCommandList* m_pID3D12GraphicsCommandList;

private:
	CCommandList(VOID);
	~CCommandList(VOID);

	BOOL Initialize(ID3D12GraphicsCommandList* pInterface);
	VOID Uninitialize(VOID);

public:
	static CCommandList* CreateInstance(ID3D12GraphicsCommandList* pInterface);
	static VOID          DestroyInstance(CCommandList* pCommandList);
};

#endif //WDK_CCOMMAND_LIST_HPP
