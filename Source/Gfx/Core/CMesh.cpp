#include "CMesh.hpp"

#include "Wdk.hpp"

#include <d3d12.h>

#include "CVertexBuffer.hpp"

CMesh::CMesh(VOID)
{
	m_pIVertexBuffer = NULL;
}

CMesh::~CMesh(VOID)
{

}

BOOL CMesh::Initialize(IVertexBuffer* pIVertexBuffer)
{
	BOOL Status = TRUE;

	m_pIVertexBuffer = pIVertexBuffer;

	return Status;
}

VOID CMesh::Uninitialize(VOID)
{
	if (m_pIVertexBuffer != NULL)
	{
		CVertexBuffer* pVertexBuffer = static_cast<CVertexBuffer*>(m_pIVertexBuffer);
		pVertexBuffer->Uninitialize();
		delete pVertexBuffer;

		m_pIVertexBuffer = NULL;
	}
}
