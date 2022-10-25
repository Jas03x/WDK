#include "CCommandBuffer.hpp"

#include <d3d12.h>

#include "Wdk.hpp"

#include "CMesh.hpp"
#include "CRenderer.hpp"

CCommandBuffer::CCommandBuffer(VOID)
{
	m_Type = COMMAND_BUFFER_TYPE_INVALID;
	m_pID3D12CommandAllocator = NULL;
	m_pID3D12CommandList = NULL;
	m_State = STATE_ERROR;
}

CCommandBuffer::~CCommandBuffer(VOID)
{

}

BOOL CCommandBuffer::Initialize(COMMAND_BUFFER_TYPE Type, ID3D12CommandAllocator* pICommandAllocator, ID3D12GraphicsCommandList* pICommandList)
{
	BOOL Status = TRUE;

	m_Type = Type;
	m_State = STATE_RESET;

	if ((pICommandAllocator != NULL) && (pICommandList != NULL))
	{
		m_pID3D12CommandAllocator = pICommandAllocator;
		m_pID3D12CommandList = pICommandList;
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
	if (m_pID3D12CommandList != NULL)
	{
		m_pID3D12CommandList->Release();
		m_pID3D12CommandList = NULL;
	}

	if (m_pID3D12CommandAllocator != NULL)
	{
		m_pID3D12CommandAllocator->Release();
		m_pID3D12CommandAllocator = NULL;
	}
}

COMMAND_BUFFER_TYPE CCommandBuffer::GetType(VOID)
{
	return m_Type;
}

ID3D12GraphicsCommandList* CCommandBuffer::GetD3D12CommandList(VOID)
{
	return m_pID3D12CommandList;
}

BOOL CCommandBuffer::Finalize(VOID)
{
	BOOL Status = TRUE;

	if (m_State == STATE_RECORDING)
	{
		if (m_pID3D12CommandList->Close() == S_OK)
		{
			m_State = STATE_CLOSED;
		}
		else
		{
			Status = FALSE;
			Console::Write(L"Error: Could not finalize command list\n");
		}
	}
	else
	{
		Status = FALSE;
		Console::Write(L"Error: Cannot finalize empty command buffer\n");
	}

	return Status;
}

BOOL CCommandBuffer::Reset(VOID)
{
	BOOL Status = TRUE;

	if (m_State != STATE_RESET)
	{
		if (m_State != STATE_ERROR)
		{
			if (m_pID3D12CommandAllocator->Reset() != S_OK)
			{
				Status = FALSE;
				Console::Write(L"Error: Failed to reset D3D12 command allocator\n");
			}

			if (Status == TRUE)
			{
				if (m_pID3D12CommandList->Reset(m_pID3D12CommandAllocator, NULL) == S_OK)
				{
					m_State = STATE_RESET;
				}
				else
				{
					Status = FALSE;
					Console::Write(L"Error: Failed to reset D3D12 command list\n");
				}
			}
		}
		else
		{
			Status = FALSE;
			Console::Write(L"Error: Cannot reset command buffer in error state\n");
		}
	}

	return Status;
}

VOID CCommandBuffer::SetViewport(UINT x, UINT y, UINT w, UINT h, FLOAT min_depth, FLOAT max_depth)
{
	if ((m_State != STATE_CLOSED) && (m_State != STATE_ERROR))
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

		m_State = STATE_RECORDING;
		m_pID3D12CommandList->RSSetViewports(1, &Viewport);
		m_pID3D12CommandList->RSSetScissorRects(1, &ScissorRect);
	}
}

VOID CCommandBuffer::SetRenderer(IRenderer* pIRenderer)
{
	if ((m_State != STATE_CLOSED) && (m_State != STATE_ERROR))
	{
		if (pIRenderer != NULL)
		{
			CRenderer* pCRenderer = static_cast<CRenderer*>(pIRenderer);

			m_State = STATE_RECORDING;
			m_pID3D12CommandList->SetPipelineState(pCRenderer->GetD3D12PipelineState());
			m_pID3D12CommandList->SetGraphicsRootSignature(pCRenderer->GetD3D12RootSignature());
		}
		else
		{
			m_State = STATE_ERROR;
			Console::Write(L"Error: Could not set command buffer renderer - received null renderer\n");
		}
	}
}

VOID CCommandBuffer::SetRenderTarget(const RenderBuffer& rBuffer)
{
	if ((m_State != STATE_CLOSED) && (m_State != STATE_ERROR))
	{
		if ((rBuffer.hResource != NULL) && (rBuffer.CpuDescriptor != 0))
		{
			D3D12_CPU_DESCRIPTOR_HANDLE CpuDescHandle = { rBuffer.CpuDescriptor };
			ID3D12Resource* pIRenderBuffer = reinterpret_cast<ID3D12Resource*>(rBuffer.hResource);
			
			D3D12_RESOURCE_BARRIER Barrier = {};
			Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			Barrier.Transition.pResource = pIRenderBuffer;
			Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

			m_State = STATE_RECORDING;
			m_pID3D12CommandList->ResourceBarrier(1, &Barrier);
			m_pID3D12CommandList->OMSetRenderTargets(1, &CpuDescHandle, FALSE, nullptr);
		}
		else
		{
			m_State = STATE_ERROR;
			Console::Write(L"Error: Could not set command buffer render target - received null render buffer\n");
		}
	}
}

VOID CCommandBuffer::Present(const RenderBuffer& rBuffer)
{
	if ((m_State != STATE_CLOSED) && (m_State != STATE_ERROR))
	{
		if (rBuffer.hResource != NULL)
		{
			ID3D12Resource* pIRenderBuffer = reinterpret_cast<ID3D12Resource*>(rBuffer.hResource);

			D3D12_RESOURCE_BARRIER Barrier = {};
			Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			Barrier.Transition.pResource = pIRenderBuffer;
			Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

			m_State = STATE_RECORDING;
			m_pID3D12CommandList->ResourceBarrier(1, &Barrier);
		}
		else
		{
			m_State = STATE_ERROR;
			Console::Write(L"Error: Could not present - received null render buffer\n");
		}
	}
}

VOID CCommandBuffer::ClearRenderBuffer(const RenderBuffer& rBuffer, CONST FLOAT RGBA[])
{
	if ((m_State != STATE_CLOSED) && (m_State != STATE_ERROR))
	{
		if ((rBuffer.CpuDescriptor != 0) && (RGBA != NULL))
		{
			D3D12_CPU_DESCRIPTOR_HANDLE CpuDescHandle = { rBuffer.CpuDescriptor };

			m_State = STATE_RECORDING;
			m_pID3D12CommandList->ClearRenderTargetView(CpuDescHandle, RGBA, 0, NULL);
		}
		else
		{
			m_State = STATE_ERROR;
			Console::Write(L"Error: Could not clear render buffer - received invalid parameter(s)\n");
		}
	}
}

VOID CCommandBuffer::Render(IMesh* pIMesh)
{
	if ((m_State != STATE_CLOSED) && (m_State != STATE_ERROR))
	{
		if (pIMesh != NULL)
		{
			CMesh* pMesh = static_cast<CMesh*>(pIMesh);
			MESH_DESC MeshDesc = pMesh->GetMeshDesc();

			D3D12_VERTEX_BUFFER_VIEW VertexBufferView = {};
			VertexBufferView.BufferLocation = pMesh->GetVertexBufferGpuVA();
			VertexBufferView.SizeInBytes = MeshDesc.BufferSize;
			VertexBufferView.StrideInBytes = MeshDesc.Stride;

			m_State = STATE_RECORDING;
			m_pID3D12CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_pID3D12CommandList->IASetVertexBuffers(0, 1, &VertexBufferView);
			m_pID3D12CommandList->DrawInstanced(MeshDesc.NumVertices, 1, 0, 0);
		}
		else
		{
			m_State = STATE_ERROR;
			Console::Write(L"Error: Could not render mesh - received invalid parameter(s)\n");
		}
	}
}
