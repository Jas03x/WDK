#ifndef WDK_CMESH_HPP
#define WDK_CMESH_HPP

#include "WdkGfx.hpp"

class CMesh : public IMesh
{
private:
	IVertexBuffer*         m_pIVertexBuffer;

	UINT                   m_NumVertices;

public:
	CMesh(VOID);
	~CMesh(VOID);

	BOOL                   Initialize(UINT NumVertices, IVertexBuffer* pIVertexBuffer);
	VOID                   Uninitialize(VOID);

	virtual UINT           GetVertexCount(VOID);
	virtual IVertexBuffer* GetVertexBuffer(VOID);
};

#endif //WDK_CMESH_HPP
