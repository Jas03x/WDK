#include "CCommandList.hpp"

#include <d3d12.h>

#include "Wdk.hpp"

CCommandList* CCommandList::CreateInstance(ID3D12GraphicsCommandList* pInterface)
{
	CCommandList* pCommandList = new CCommandList();

	if (pCommandList != NULL)
	{
		if (!pCommandList->Initialize(pInterface))
		{
			DeleteInstance(pCommandList);
			pCommandList = NULL;
		}
	}

	return pCommandList;
}

VOID CCommandList::DeleteInstance(CCommandList* pCommandList)
{
	if (pCommandList != NULL)
	{
		pCommandList->Uninitialize();
		delete pCommandList;
	}
}

CCommandList::CCommandList(VOID)
{
	m_pInterface = NULL;
}

CCommandList::~CCommandList(VOID)
{

}

BOOL CCommandList::Initialize(ID3D12GraphicsCommandList* pInterface)
{
	BOOL Status = TRUE;

	if (pInterface != NULL)
	{
		m_pInterface = pInterface;
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
	if (m_pInterface != NULL)
	{
		m_pInterface->Release();
		m_pInterface = NULL;
	}
}
