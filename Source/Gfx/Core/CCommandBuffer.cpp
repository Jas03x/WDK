#include "CCommandBuffer.hpp"

#include <d3d12.h>

#include "Wdk.hpp"

#include "CConstantBuffer.hpp"
#include "CMesh.hpp"
#include "CRendererState.hpp"

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
		Console::Write(L"Error: Could not create command buffer - null interfaces\n");
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

ID3D12GraphicsCommandList* CCommandBuffer::GetD3D12Interface(VOID)
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

VOID CCommandBuffer::ProgramPipeline(IRendererState* pIRendererState)
{
	if ((m_State != STATE_CLOSED) && (m_State != STATE_ERROR))
	{
		if (pIRendererState != NULL)
		{
			CRendererState* pRendererState = static_cast<CRendererState*>(pIRendererState);
			ID3D12DescriptorHeap* pHeaps[] = { pRendererState->GetShaderResourceHeap() };

			m_State = STATE_RECORDING;
			m_pID3D12CommandList->SetPipelineState(pRendererState->GetD3D12PipelineState());
			m_pID3D12CommandList->SetGraphicsRootSignature(pRendererState->GetD3D12RootSignature());
			m_pID3D12CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_pID3D12CommandList->SetDescriptorHeaps(_countof(pHeaps), pHeaps);
		}
		else
		{
			Console::Write(L"Error: Could not set command buffer renderer - received null renderer\n");
			m_State = STATE_ERROR;
		}
	}
}

VOID CCommandBuffer::SetConstantBuffer(UINT Index, IConstantBuffer* pIConstantBuffer)
{
	if ((m_State != STATE_CLOSED) && (m_State != STATE_ERROR))
	{
		if (pIConstantBuffer != NULL)
		{
			CConstantBuffer* pConstantBuffer = static_cast<CConstantBuffer*>(pIConstantBuffer);

			m_State = STATE_RECORDING;
			m_pID3D12CommandList->SetGraphicsRootConstantBufferView(0, pConstantBuffer->GetGpuVA());
		}
		else
		{
			Console::Write(L"Error: Could not set command buffer renderer - received null renderer\n");
			m_State = STATE_ERROR;
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

VOID CCommandBuffer::SetVertexBuffers(UINT NumBuffers, CONST IVertexBuffer* pIVertexBuffers)
{
	enum { MAX_VERTEX_BUFFERS = 8 };

	if ((m_State != STATE_CLOSED) && (m_State != STATE_ERROR))
	{
		bool status = true;

		if (pIVertexBuffers == NULL)
		{
			Console::Write(L"Error: Null descriptors\n");
			status = false;
		}

		if (status && (NumBuffers > MAX_VERTEX_BUFFERS))
		{
			Console::Write(L"Error: More vertex buffers than supported\n");
			status = false;
		}

		if (status)
		{
			D3D12_VERTEX_BUFFER_VIEW VertexBufferViews[MAX_VERTEX_BUFFERS] = {};

			for (UINT i = 0; i < NumBuffers; i++)
			{
				CONST VERTEX_BUFFER_DESC& Desc = pIVertexBuffers[i].GetDesc();

				VertexBufferViews[i].BufferLocation = Desc.GpuVA;
				VertexBufferViews[i].SizeInBytes = Desc.Size;
				VertexBufferViews[i].StrideInBytes = Desc.Stride;
			}

			m_State = STATE_RECORDING;
			m_pID3D12CommandList->IASetVertexBuffers(0, NumBuffers, VertexBufferViews);
		}
		else
		{
			Console::Write(L"Error: Failed to set vertex buffer(s)\n");
			m_State = STATE_ERROR;
		}
	}
}

VOID CCommandBuffer::Draw(UINT NumVertices)
{
	if ((m_State != STATE_CLOSED) && (m_State != STATE_ERROR))
	{
		m_State = STATE_RECORDING;
		m_pID3D12CommandList->DrawInstanced(NumVertices, 1, 0, 0);
	}
}
