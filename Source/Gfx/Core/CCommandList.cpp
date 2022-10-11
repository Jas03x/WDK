#include "CCommandList.hpp"

#include <d3d12.h>

#include "Wdk.hpp"

CCommandList::CCommandList(VOID)
{
	m_pID3D12GraphicsCommandList = NULL;
}

CCommandList::~CCommandList(VOID)
{

}

BOOL CCommandList::Initialize(ID3D12GraphicsCommandList* pInterface)
{
	BOOL Status = TRUE;

	if (pInterface != NULL)
	{
		m_pID3D12GraphicsCommandList = pInterface;
	}
	else
	{
		Status = FALSE;
		Console::Write(L"Error: Could not create command list - null handle\n");
	}

	return Status;
}

VOID CCommandList::Uninitialize(VOID)
{
	if (m_pID3D12GraphicsCommandList != NULL)
	{
		m_pID3D12GraphicsCommandList->Release();
		m_pID3D12GraphicsCommandList = NULL;
	}
}
