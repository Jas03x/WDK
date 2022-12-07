#ifndef WDK_CMESH_HPP
#define WDK_CMESH_HPP

#include "WdkGfx.hpp"

class CMesh : public IMesh
{
private:
	IVertexBuffer* m_pIVertexBuffer;

public:
	CMesh(VOID);
	~CMesh(VOID);

	BOOL Initialize(IVertexBuffer* pIVertexBuffer);
	VOID Uninitialize(VOID);
};

#endif //WDK_CMESH_HPP
