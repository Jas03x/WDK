#include "CConstantBuffer.hpp"

#include "Wdk.hpp"

#include <d3d12.h>

CConstantBuffer::CConstantBuffer(VOID)
{
	m_CpuVA = NULL;
	m_GpuVA = 0;
	m_pID3D12Resource = NULL;
}

CConstantBuffer::~CConstantBuffer(VOID)
{

}

BOOL CConstantBuffer::Initialize(ID3D12Resource* pID3D12ConstantBufferResource, VOID* CpuVA)
{
	BOOL Status = TRUE;

	if (pID3D12ConstantBufferResource != NULL)
	{
		m_CpuVA = CpuVA;
		m_GpuVA = pID3D12ConstantBufferResource->GetGPUVirtualAddress();
		m_pID3D12Resource = pID3D12ConstantBufferResource;
	}
	else
	{
		Status = FALSE;
		Console::Write(L"Error: Failed to initialize constant buffer\n");
	}

	return Status;
}

VOID CConstantBuffer::Uninitialize(VOID)
{
	if (m_pID3D12Resource != NULL)
	{
		m_pID3D12Resource->Release();
		m_pID3D12Resource = NULL;
	}
}

VOID* CConstantBuffer::GetCpuVA(VOID)
{
	return m_CpuVA;
}

UINT64 CConstantBuffer::GetGpuVA(VOID)
{
	return m_GpuVA;
}
