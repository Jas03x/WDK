#include "CCommandBuffer.hpp"

#include <d3d12.h>

#include "Wdk.hpp"

#include "CMesh.hpp"
#include "CRenderer.hpp"

CCommandBuffer::CCommandBuffer(VOID)
{
	m_pID3D12CommandAllocator = NULL;
	m_pID3D12GraphicsCommandList = NULL;
	m_bWriteStatus = FALSE;
}

CCommandBuffer::~CCommandBuffer(VOID)
{

}

BOOL CCommandBuffer::Initialize(ID3D12CommandAllocator* pICommandAllocator, ID3D12GraphicsCommandList* pIGraphicsCommandList)
{
	BOOL Status = TRUE;

	m_bWriteStatus = TRUE;

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

BOOL CCommandBuffer::Finalize(VOID)
{
	BOOL Status = TRUE;

	if (m_bWriteStatus == TRUE)
	{
		if (m_pID3D12GraphicsCommandList->Close() != S_OK)
		{
			Status = FALSE;
			Console::Write(L"Error: Could not finalize command list\n");
		}
	}
	else
	{
		Status = FALSE;
		Console::Write(L"Error: Error while generating command buffer\n");
	}

	return Status;
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

	if (Status == FALSE)
	{
		m_bWriteStatus = FALSE;
	}

	return Status;
}

VOID CCommandBuffer::SetViewport(UINT x, UINT y, UINT w, UINT h, FLOAT min_depth, FLOAT max_depth)
{
	if (m_bWriteStatus == TRUE)
	{
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
	}
}

VOID CCommandBuffer::SetRenderer(IRenderer* pIRenderer)
{
	if (m_bWriteStatus == TRUE)
	{
		if (pIRenderer != NULL)
		{
			CRenderer* pCRenderer = static_cast<CRenderer*>(pIRenderer);

			m_pID3D12GraphicsCommandList->SetPipelineState(pCRenderer->GetD3D12PipelineState());
			m_pID3D12GraphicsCommandList->SetGraphicsRootSignature(pCRenderer->GetD3D12RootSignature());
		}
		else
		{
			m_bWriteStatus = FALSE;
			Console::Write(L"Error: Could not set command buffer renderer - received null renderer\n");
		}
	}
}

VOID CCommandBuffer::SetRenderTarget(HANDLE hResource, UINT64 CpuDescriptor)
{
	if (m_bWriteStatus == TRUE)
	{
		if ((hResource != NULL) && (CpuDescriptor != 0))
		{
			D3D12_CPU_DESCRIPTOR_HANDLE CpuDescHandle = { CpuDescriptor };
			ID3D12Resource* pIRenderBuffer = reinterpret_cast<ID3D12Resource*>(hResource);
			
			D3D12_RESOURCE_BARRIER Barrier = {};
			Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			Barrier.Transition.pResource = pIRenderBuffer;
			Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

			m_pID3D12GraphicsCommandList->ResourceBarrier(1, &Barrier);
			m_pID3D12GraphicsCommandList->OMSetRenderTargets(1, &CpuDescHandle, FALSE, nullptr);
		}
		else
		{
			m_bWriteStatus = FALSE;
			Console::Write(L"Error: Could not set command buffer render target - received null render buffer\n");
		}
	}
}

VOID CCommandBuffer::Present(HANDLE hResource)
{
	if (m_bWriteStatus == TRUE)
	{
		if (hResource != NULL)
		{
			ID3D12Resource* pIRenderBuffer = reinterpret_cast<ID3D12Resource*>(hResource);

			D3D12_RESOURCE_BARRIER Barrier = {};
			Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			Barrier.Transition.pResource = pIRenderBuffer;
			Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

			m_pID3D12GraphicsCommandList->ResourceBarrier(1, &Barrier);
		}
		else
		{
			m_bWriteStatus = FALSE;
			Console::Write(L"Error: Could not present - received null render buffer\n");
		}
	}
}

VOID CCommandBuffer::ClearRenderBuffer(UINT64 CpuDescriptor, FLOAT RGBA[])
{
	if (m_bWriteStatus == TRUE)
	{
		if ((CpuDescriptor != 0) && (RGBA != NULL))
		{
			D3D12_CPU_DESCRIPTOR_HANDLE CpuDescHandle = { CpuDescriptor };

			m_pID3D12GraphicsCommandList->ClearRenderTargetView(CpuDescHandle, RGBA, 0, NULL);
		}
		else
		{
			m_bWriteStatus = FALSE;
			Console::Write(L"Error: Could not clear render buffer - received invalid parameter(s)\n");
		}
	}
}

VOID CCommandBuffer::Render(IMesh* pIMesh)
{
	if (m_bWriteStatus == TRUE)
	{
		if (pIMesh != NULL)
		{
			CMesh* pMesh = static_cast<CMesh*>(pIMesh);
			MESH_DESC MeshDesc = pMesh->GetMeshDesc();

			D3D12_VERTEX_BUFFER_VIEW VertexBufferView = {};
			VertexBufferView.BufferLocation = pMesh->GetVertexBufferGpuVA();
			VertexBufferView.SizeInBytes = MeshDesc.BufferSize;
			VertexBufferView.StrideInBytes = MeshDesc.Stride;

			m_pID3D12GraphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_pID3D12GraphicsCommandList->IASetVertexBuffers(0, 1, &VertexBufferView);
			m_pID3D12GraphicsCommandList->DrawInstanced(MeshDesc.NumVertices, 1, 0, 0);
		}
		else
		{
			m_bWriteStatus = FALSE;
			Console::Write(L"Error: Could not render mesh - received invalid parameter(s)\n");
		}
	}
}
