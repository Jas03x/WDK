#ifndef WDK_CMESH_HPP
#define WDK_CMESH_HPP

#include "WdkGfx.hpp"

class CMesh : public IMesh
{
private:
	IVertexBuffer*         m_pIVertexBuffer;

	uint32_t               m_NumVertices;

public:
	CMesh(void);
	~CMesh(void);

	bool                   Initialize(uint32_t NumVertices, IVertexBuffer* pIVertexBuffer);
	void                   Uninitialize(void);

	virtual uint32_t       GetVertexCount(void);
	virtual IVertexBuffer* GetVertexBuffer(void);
};

#endif //WDK_CMESH_HPP
