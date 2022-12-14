#ifndef WDK_CCONSTANT_BUFFER_HPP
#define WDK_CCONSTANT_BUFFER_HPP

#include "WdkGfx.hpp"

struct ID3D12Resource;

class CConstantBuffer : public IConstantBuffer
{
private:
	ID3D12Resource* m_pID3D12Resource;

	VOID*  m_CpuVA;
	UINT64 m_GpuVA;

public:
	CConstantBuffer(VOID);
	~CConstantBuffer(VOID);

	BOOL Initialize(ID3D12Resource* pID3D12ConstantBufferResource, VOID* CpuVA);
	VOID Uninitialize(VOID);

	virtual VOID*  GetCpuVA(VOID);
	virtual UINT64 GetGpuVA(VOID);
};

#endif // WDK_CCONSTANT_BUFFER_HPP
