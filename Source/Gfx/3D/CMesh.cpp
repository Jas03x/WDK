#include "CMesh.hpp"

#include <d3d12.h>

CMesh::CMesh(VOID)
{

}

CMesh::~CMesh(VOID)
{

}

BOOL CMesh::Initialize(ID3D12Resource* VertexBuffer, UINT SizeInBytes, UINT StrideInBytes)
{
	BOOL Status = TRUE;

	m_pID3D12Resource = VertexBuffer;

	m_VertexBufferGpuVA = m_pID3D12Resource->GetGPUVirtualAddress();
	m_VertexBufferSizeInBytes = SizeInBytes;
	m_VertexBufferStrideInBytes = StrideInBytes;

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
