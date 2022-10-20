#include "CCommandQueue.hpp"

#include <d3d12.h>

#include "Wdk.hpp"

#include "CCommandBuffer.hpp"

CCommandQueue::CCommandQueue(VOID)
{
	m_pID3D12CommandQueue = NULL;
	m_pID3D12Fence = NULL;

	m_hFenceEvent = NULL;
	m_FenceValue = 0;
}

CCommandQueue::~CCommandQueue(VOID)
{
}

BOOL CCommandQueue::Initialize(COMMAND_QUEUE_TYPE Type, ID3D12CommandQueue* pICommandQueue, ID3D12Fence* pIFence)
{
	BOOL Status = TRUE;

	m_Type = Type;
	m_hFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (m_hFenceEvent != NULL)
	{
		m_FenceValue = 1;
	}
	else
	{
		Status = FALSE;
		Console::Write(L"Error: could not initialize fence event handle\n");
	}

	if (Status == TRUE)
	{
		if ((pICommandQueue != NULL) && (pIFence != NULL))
		{
			m_pID3D12CommandQueue = pICommandQueue;
			m_pID3D12Fence = pIFence;
		}
		else
		{
			Status = FALSE;
			Console::Write(L"Error: could not initialize command queue - received null d3d12 interface(s)\n");
		}
	}

	return Status;
}

VOID CCommandQueue::Uninitialize(VOID)
{
	if (m_pID3D12CommandQueue != NULL)
	{
		m_pID3D12CommandQueue->Release();
		m_pID3D12CommandQueue = NULL;
	}

	if (m_pID3D12Fence != NULL)
	{
		m_pID3D12Fence->Release();
		m_pID3D12Fence = NULL;
	}

	if (m_hFenceEvent != NULL)
	{
		CloseHandle(m_hFenceEvent);
		m_hFenceEvent = NULL;
	}
}

ID3D12CommandQueue* CCommandQueue::GetD3D12CommandQueue(VOID)
{
	return m_pID3D12CommandQueue;
}

BOOL CCommandQueue::SubmitCommandBuffer(ICommandBuffer* pICommandBuffer)
{
	BOOL Status = TRUE;

	if (pICommandBuffer == NULL)
	{
		Status = FALSE;
	}

	if (Status == TRUE)
	{
		CCommandBuffer* pCommandBuffer = static_cast<CCommandBuffer*>(pICommandBuffer);

		if (pCommandBuffer->GetType() == m_Type)
		{
			ID3D12CommandList* pICommandLists[] = { pCommandBuffer->GetD3D12CommandList() };
			m_pID3D12CommandQueue->ExecuteCommandLists(1, pICommandLists);
		}
		else
		{
			Status = FALSE;
			Console::Write(L"Error: Submitting different command buffer type to command queue\n");
		}
	}

	return Status;
}

BOOL CCommandQueue::Sync(VOID)
{
	BOOL Status = TRUE;

	if (m_pID3D12CommandQueue->Signal(m_pID3D12Fence, m_FenceValue) != S_OK)
	{
		Status = FALSE;
		Console::Write(L"Error: Failed to signal fence from command queue\n");
	}

	if (Status == TRUE)
	{
		if (m_pID3D12Fence->GetCompletedValue() < m_FenceValue)
		{
			if (m_pID3D12Fence->SetEventOnCompletion(m_FenceValue, m_hFenceEvent) == S_OK)
			{
				DWORD Result = WaitForSingleObject(m_hFenceEvent, COMMAND_QUEUE_TIMEOUT);

				switch (Result)
				{
				case WAIT_OBJECT_0:
				{
					break;
				}
				case WAIT_TIMEOUT:
				{
					Status = FALSE;
					Console::Write(L"Error: command queue submission timed out\n");
					break;
				}
				case WAIT_FAILED:
				{
					Status = FALSE;
					Console::Write(L"Error: failed to wait for command queue submission\n");
					break;
				}
				default:
				{
					Status = FALSE;
					Console::Write(L"Error: unknown error occurred while waiting for command queue submission\n");
					break;
				}
				}
			}
			else
			{
				Status = FALSE;
				Console::Write(L"Error: failed to set fence event on completition\n");
			}
		}
	}

	m_FenceValue++;

	return Status;
}
