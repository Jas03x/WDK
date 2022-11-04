#ifndef WDK_CCONSTANT_BUFFER_HPP
#define WDK_CCONSTANT_BUFFER_HPP

#include "WdkGfx.hpp"

struct ID3D12Resource;

class CConstantBuffer : public IConstantBuffer
{
private:
	ID3D12Resource* m_pID3D12ConstantBufferResource;

	VOID* m_CpuVa;

public:
	CConstantBuffer(VOID);
	~CConstantBuffer(VOID);

	BOOL Initialize(ID3D12Resource* pID3D12ConstantBufferResource, VOID* CpuVa);
	VOID Uninitialize(VOID);
};

#endif // WDK_CCONSTANT_BUFFER_HPP
