#ifndef WDK_CVERTEX_BUFFER_HPP
#define WDK_CVERTEX_BUFFER_HPP

#include "WdkGfx.hpp"

struct ID3D12Resource;

class CVertexBuffer : public IVertexBuffer
{
	VERTEX_BUFFER_DESC m_Desc;

	UINT64             m_VertexBufferGpuVA;

	ID3D12Resource*    m_pID3D12Resource;

public:
	CVertexBuffer(VOID);
	~CVertexBuffer(VOID);

	BOOL                      Initialize(ID3D12Resource* pID3D12VertexBuffer, CONST VERTEX_BUFFER_DESC& rDesc);
	VOID                      Uninitialize(VOID);

	UINT64                    GetGpuVA(VOID);
	CONST VERTEX_BUFFER_DESC& GetDesc(VOID);
};

#endif // WDK_CVERTEX_BUFFER_HPP
