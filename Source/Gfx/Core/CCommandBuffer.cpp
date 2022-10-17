#include "CCommandBuffer.hpp"

#include <d3d12.h>

#include "Wdk.hpp"

#include "CRenderer.hpp"

CCommandBuffer::CCommandBuffer(VOID)
{
	m_bIsFirstSubmission = FALSE;

	m_pID3D12CommandAllocator = NULL;
	m_pID3D12GraphicsCommandList = NULL;
}

CCommandBuffer::~CCommandBuffer(VOID)
{

}

BOOL CCommandBuffer::Initialize(ID3D12CommandAllocator* pICommandAllocator, ID3D12GraphicsCommandList* pIGraphicsCommandList)
{
	BOOL Status = TRUE;

	if ((pICommandAllocator != NULL) && (pIGraphicsCommandList != NULL))
	{
		m_pID3D12CommandAllocator = pICommandAllocator;
		m_pID3D12GraphicsCommandList = pIGraphicsCommandList;
	}
	else
	{
		Status = FALSE;
		Console::Write(L"Error: Could not create command buffer - null D3D12 interfaces\n");
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

	if (m_pID3D12CommandAllocator != NULL)
	{
		m_pID3D12CommandAllocator->Release();
		m_pID3D12CommandAllocator = NULL;
	}
}

ID3D12GraphicsCommandList* CCommandBuffer::GetD3D12GraphicsCommandList(VOID)
{
	return m_pID3D12GraphicsCommandList;
}

BOOL CCommandBuffer::Reset(VOID)
{
	BOOL Status = TRUE;

	if (m_pID3D12CommandAllocator->Reset() != S_OK)
	{
		Status = FALSE;
		Console::Write(L"Error: Failed to reset D3D12 command allocator\n");
	}

	if (Status == TRUE)
	{
		if (m_pID3D12GraphicsCommandList->Reset(m_pID3D12CommandAllocator, NULL) != S_OK)
		{
			Status = FALSE;
			Console::Write(L"Error: Failed to reset D3D12 command list\n");
		}
	}

	return Status;
}

BOOL CCommandBuffer::SetViewport(UINT x, UINT y, UINT w, UINT h, FLOAT min_depth, FLOAT max_depth)
{
	BOOL Status = TRUE;

	D3D12_VIEWPORT Viewport = {};
	Viewport.TopLeftX = static_cast<FLOAT>(x);
	Viewport.TopLeftY = static_cast<FLOAT>(y);
	Viewport.Width = static_cast<FLOAT>(w);
	Viewport.Height = static_cast<FLOAT>(h);
	Viewport.MinDepth = min_depth;
	Viewport.MaxDepth = max_depth;

	D3D12_RECT ScissorRect = {};
	ScissorRect.left = x;
	ScissorRect.top = y;
	ScissorRect.right = w;
	ScissorRect.bottom = h;

	m_pID3D12GraphicsCommandList->RSSetViewports(1, &Viewport);
	m_pID3D12GraphicsCommandList->RSSetScissorRects(1, &ScissorRect);

	return Status;
}

BOOL CCommandBuffer::SetRenderer(IRenderer* pIRenderer)
{
	BOOL Status = TRUE;

	if (pIRenderer != NULL)
	{
		CRenderer* pCRenderer = static_cast<CRenderer*>(pIRenderer);

		m_pID3D12GraphicsCommandList->SetPipelineState(pCRenderer->GetD3D12PipelineState());
		m_pID3D12GraphicsCommandList->SetGraphicsRootSignature(pCRenderer->GetD3D12RootSignature());
	}
	else
	{
		Status = FALSE;
		Console::Write(L"Error: Null renderer\n");
	}

	return Status;
}
