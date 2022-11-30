#include "CMesh.hpp"

#include <d3d12.h>

CMesh::CMesh(VOID)
{
	m_VertexBufferGpuVA = 0;
	m_pID3D12Resource = NULL;
	ZeroMemory(&m_MeshDesc, sizeof(MESH_DESC));
}

CMesh::~CMesh(VOID)
{

}

BOOL CMesh::Initialize(ID3D12Resource* VertexBuffer, MESH_DESC& rDesc)
{
	BOOL Status = TRUE;

	m_pID3D12Resource = VertexBuffer;

	m_MeshDesc = rDesc;

	m_VertexBufferGpuVA = m_pID3D12Resource->GetGPUVirtualAddress();

	return Status;
}

VOID CMesh::Uninitialize(VOID)
{
	if (m_pID3D12Resource != NULL)
	{
		m_pID3D12Resource->Release();
		m_pID3D12Resource = NULL;
	}
}

UINT64 CMesh::GetVertexBufferGpuVA(VOID)
{
	return m_VertexBufferGpuVA;
}

MESH_DESC CMesh::GetMeshDesc(VOID)
{
	return m_MeshDesc;
}
