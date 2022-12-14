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

BOOL CMesh::Initialize(UINT NumVertices, IVertexBuffer* pIVertexBuffer)
{
	BOOL Status = TRUE;

	m_NumVertices = NumVertices;
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

UINT CMesh::GetVertexCount(VOID)
{
	return m_NumVertices;
}

IVertexBuffer* CMesh::GetVertexBuffer(VOID)
{
	return m_pIVertexBuffer;
}
