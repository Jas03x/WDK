#ifndef WDK_CCOMMAND_BUFFER_HPP
#define WDK_CCOMMAND_BUFFER_HPP

#include "WdkGfx.hpp"

struct ID3D12GraphicsCommandList;

class CCommandBuffer : public ICommandBuffer
{
private:
	ID3D12GraphicsCommandList* m_pID3D12GraphicsCommandList;

public:
	CCommandBuffer(VOID);
	~CCommandBuffer(VOID);

	BOOL Initialize(ID3D12GraphicsCommandList* pID3D12GraphicsCommandList);
	VOID Uninitialize(VOID);

	virtual HANDLE GetHandle();
};

#endif // WDK_CCOMMAND_BUFFER_HPP
