#ifndef WDK_CMESH_HPP
#define WDK_CMESH_HPP

#include "WdkGfx.hpp"

struct ID3D12Resource;

class CMesh : public IMesh
{
private:
	ID3D12Resource* m_pID3D12Resource;

	MESH_DESC       m_MeshDesc;

	UINT64          m_VertexBufferGpuVA;

public:
	CMesh(VOID);
	~CMesh(VOID);

	BOOL Initialize(ID3D12Resource* VertexBuffer, MESH_DESC& rDesc);
	VOID Uninitialize(VOID);

	UINT64 GetVertexBufferGpuVA(VOID);
	MESH_DESC GetMeshDesc(VOID);
};

#endif //WDK_CMESH_HPP
