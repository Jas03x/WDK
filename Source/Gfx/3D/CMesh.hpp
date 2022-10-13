#ifndef WDK_CMESH_HPP
#define WDK_CMESH_HPP

#include "WdkGfx.hpp"

struct ID3D12Resource;

class CMesh : public IMesh
{
private:
	ID3D12Resource* m_pID3D12Resource;

	UINT64          m_VertexBufferGpuVA;
	UINT32          m_VertexBufferSizeInBytes;
	UINT32          m_VertexBufferStrideInBytes;

public:
	CMesh(VOID);
	~CMesh(VOID);

	BOOL Initialize(ID3D12Resource* VertexBuffer, UINT SizeInBytes, UINT StrideInBytes);
	VOID Uninitialize(VOID);
};

#endif //WDK_CMESH_HPP
