#include "CFence.hpp"

#include <d3d12.h>

#include "Wdk.hpp"

CFence* CFence::CreateInstance(ID3D12Fence* pInterface)
{
	CFence* pFence = new CFence();

	if (pFence != NULL)
	{
		if (!pFence->Initialize(pInterface))
		{
			DestroyInstance(pFence);
			pFence = NULL;
		}
	}

	return pFence;
}

VOID CFence::DestroyInstance(CFence* pFence)
{
	if (pFence != NULL)
	{
		pFence->Uninitialize();
		delete pFence;
	}
}

CFence::CFence(VOID)
{
	m_pID3D12Fence = NULL;
	m_hFenceEvent = NULL;
}

CFence::~CFence(VOID)
{
}

BOOL CFence::Initialize(ID3D12Fence* pInterface)
{
	BOOL Status = TRUE;

	if (pInterface != NULL)
	{
		m_pID3D12Fence = pInterface;
	}
	else
	{
		Status = FALSE;
		Console::Write(L"Error: could not create fence object - invalid dx12 fence interface\n");
	}

	if (Status == TRUE)
	{
		m_hFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		if (m_hFenceEvent == NULL)
		{
			Status = FALSE;
			Console::Write(L"Error: could not initialize fence event handle\n");
		}
	}

	return Status;
}

VOID CFence::Uninitialize(VOID)
{
	if (m_hFenceEvent != NULL)
	{
		CloseHandle(m_hFenceEvent);
		m_hFenceEvent = NULL;
	}

	if (m_pID3D12Fence != NULL)
	{
		m_pID3D12Fence->Release();
		m_pID3D12Fence = NULL;
	}
}
