#include "CVertexBuffer.hpp"

#include "Windows.h"

#include <d3d12.h>

CVertexBuffer::CVertexBuffer(VOID)
{
	m_VertexBufferGpuVA = 0;
	m_pID3D12Resource = NULL;

	ZeroMemory(&m_Desc, sizeof(VERTEX_BUFFER_DESC));
}

CVertexBuffer::~CVertexBuffer(VOID)
{

}

BOOL CVertexBuffer::Initialize(ID3D12Resource* pID3D12VertexBuffer, CONST VERTEX_BUFFER_DESC& rDesc)
{
	BOOL Status = TRUE;

	if (pID3D12VertexBuffer != NULL)
	{
		m_pID3D12Resource = pID3D12VertexBuffer;
		m_VertexBufferGpuVA = m_pID3D12Resource->GetGPUVirtualAddress();
		CopyMemory(&m_Desc, &rDesc, sizeof(VERTEX_BUFFER_DESC));
	}
	else
	{
		Status = FALSE;
	}

	return Status;
}

VOID CVertexBuffer::Uninitialize(VOID)
{
	m_VertexBufferGpuVA = 0;
	ZeroMemory(&m_Desc, sizeof(VERTEX_BUFFER_DESC));

	if (m_pID3D12Resource != NULL)
	{
		m_pID3D12Resource->Release();
		m_pID3D12Resource = NULL;
	}
}

UINT64 CVertexBuffer::GetGpuVA(VOID)
{
	return m_VertexBufferGpuVA;
}

CONST VERTEX_BUFFER_DESC& CVertexBuffer::GetDesc(VOID)
{
	return m_Desc;
}
