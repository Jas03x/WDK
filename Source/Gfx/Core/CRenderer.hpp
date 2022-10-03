#ifndef WDK_CRENDERER_HPP
#define WDK_CRENDERER_HPP

#include "Wdk.hpp"
#include "WdkGfx.hpp"

struct ID3D12PipelineState;

class CRenderer : public IRenderer
{
private:
	ID3D12PipelineState* m_pID3D12PipelineState;

protected:
	CRenderer(VOID);
	~CRenderer(VOID);

	BOOL Initialize(ID3D12PipelineState* pIPipelineState);
	VOID Uninitialize(VOID);

public:
	static CRenderer* CreateInstance(ID3D12PipelineState* pIPipelineState);
	static VOID       DestroyInstance(CRenderer* pRenderer);
};

BOOL ReadShaderBytecode(const FILE_PATH& Path, SHADER_BYTECODE& rDesc);
VOID ReleaseShaderBytecode(SHADER_BYTECODE& rDesc);

#endif // WDK_CRENDERER_HPP
