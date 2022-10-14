#include "CCommandBuffer.hpp"

#include <d3d12.h>

#include "Wdk.hpp"

CCommandBuffer::CCommandBuffer(VOID)
{
	m_pID3D12GraphicsCommandList = NULL;
}

CCommandBuffer::~CCommandBuffer(VOID)
{

}

BOOL CCommandBuffer::Initialize(ID3D12GraphicsCommandList* pID3D12GraphicsCommandList)
{
	BOOL Status = TRUE;

	if (pID3D12GraphicsCommandList != NULL)
	{
		m_pID3D12GraphicsCommandList = pID3D12GraphicsCommandList;
	}
	else
	{
		Status = FALSE;
		Console::Write(L"Error: Could not create command list - null handle\n");
	}

	return Status;
}

VOID CCommandBuffer::Uninitialize(VOID)
{
	if (m_pID3D12GraphicsCommandList != NULL)
	{
		m_pID3D12GraphicsCommandList->Release();
		m_pID3D12GraphicsCommandList = NULL;
	}
}

ID3D12GraphicsCommandList* CCommandBuffer::GetD3D12GraphicsCommandList()
{
	return m_pID3D12GraphicsCommandList;
}
