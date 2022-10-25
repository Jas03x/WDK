#include "CSwapChain.hpp"

#include <d3d12.h>
#include <dxgi1_6.h>

#include "Wdk.hpp"

CSwapChain::CSwapChain(VOID)
{
	m_FrameIndex = 0;
	m_RtvDescriptorIncrement = 0;

	m_pIDxgiSwapChain = NULL;
	m_pID3D12DescriptorHeap = NULL;

	for (UINT i = 0; i < NUM_BUFFERS; i++)
	{
		m_pID3D12RenderBuffers[i] = NULL;
		m_RenderBufferCpuDescriptors[i] = 0;
	}
}

CSwapChain::~CSwapChain(VOID)
{

}

BOOL CSwapChain::Initialize(IDXGISwapChain4* pIDxgiSwapChain, ID3D12DescriptorHeap* pIRtvDescriptorHeap, const Descriptor& rDesc)
{
	BOOL Status = TRUE;

	if ((pIDxgiSwapChain != NULL) && (pIRtvDescriptorHeap != NULL))
	{
		m_pIDxgiSwapChain = pIDxgiSwapChain;
		m_pID3D12DescriptorHeap = pIRtvDescriptorHeap;

		for (UINT i = 0; i < NUM_BUFFERS; i++)
		{
			m_RenderBufferCpuDescriptors[i] = rDesc.RenderBuffers[i].CpuDescriptor;
			m_pID3D12RenderBuffers[i] = reinterpret_cast<ID3D12Resource*>(rDesc.RenderBuffers[i].hResource);
		}

		m_RtvDescriptorIncrement = rDesc.RtvDescriptorIncrement;
		m_FrameIndex = m_pIDxgiSwapChain->GetCurrentBackBufferIndex();
	}
	else
	{
		Status = FALSE;
		Console::Write(L"Error: NULL swapchain parameters\n");
	}

	return Status;
}

VOID CSwapChain::Uninitialize(VOID)
{
	for (UINT32 i = 0; i < NUM_BUFFERS; i++)
	{
		if (m_pID3D12RenderBuffers[i] != NULL)
		{
			m_pID3D12RenderBuffers[i]->Release();
			m_pID3D12RenderBuffers[i] = NULL;
		}
	}

	if (m_pID3D12DescriptorHeap != NULL)
	{
		m_pID3D12DescriptorHeap->Release();
		m_pID3D12DescriptorHeap = NULL;
	}

	if (m_pIDxgiSwapChain != NULL)
	{
		m_pIDxgiSwapChain->Release();
		m_pIDxgiSwapChain = NULL;
	}
}

UINT CSwapChain::GetNumBuffers(VOID)
{
	return NUM_BUFFERS;
}

VOID CSwapChain::GetCurrentRenderBuffer(HANDLE& hResource, UINT64& CpuDescriptor)
{
	hResource = m_pID3D12RenderBuffers[m_FrameIndex];
	CpuDescriptor = m_RenderBufferCpuDescriptors[m_FrameIndex];
}

BOOL CSwapChain::Present(VOID)
{
	BOOL Status = TRUE;

	if (m_pIDxgiSwapChain->Present(1, 0) == S_OK)
	{
		m_FrameIndex = (m_FrameIndex + 1) % NUM_BUFFERS;
	}
	else
	{
		Status = FALSE;
		Console::Write(L"Error: Failed to signal swap chain to present\n");
	}

	return Status;
}
