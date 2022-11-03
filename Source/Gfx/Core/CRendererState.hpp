#ifndef WDK_CRENDERER_STATE_HPP
#define WDK_CRENDERER_STATE_HPP

#include "Wdk.hpp"
#include "WdkGfx.hpp"

struct ID3D12PipelineState;
struct ID3D12RootSignature;

class CRendererState : public IRendererState
{
private:
	ID3D12PipelineState* m_pID3D12PipelineState;
	ID3D12RootSignature* m_pID3D12RootSignature;

public:
	CRendererState(VOID);
	~CRendererState(VOID);

	BOOL Initialize(ID3D12RootSignature* pIRootSignature, ID3D12PipelineState* pIPipelineState);
	VOID Uninitialize(VOID);

	ID3D12PipelineState* GetD3D12PipelineState(VOID);
	ID3D12RootSignature* GetD3D12RootSignature(VOID);
};

BOOL ReadShaderBytecode(CONST FILE_PATH& Path, SHADER_BYTECODE& rDesc);
VOID ReleaseShaderBytecode(SHADER_BYTECODE& rDesc);

#endif // WDK_CRENDERER_STATE_HPP
