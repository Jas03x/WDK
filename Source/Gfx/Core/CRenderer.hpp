#ifndef WDK_CRENDERER_HPP
#define WDK_CRENDERER_HPP

#include "Wdk.hpp"
#include "WdkGfx.hpp"

struct ID3D12PipelineState;
struct ID3D12RootSignature;

class CRenderer : public IRenderer
{
private:
	ID3D12PipelineState* m_pID3D12PipelineState;
	ID3D12RootSignature* m_pID3D12RootSignature;

public:
	CRenderer(VOID);
	~CRenderer(VOID);

	BOOL Initialize(ID3D12RootSignature* pIRootSignature, ID3D12PipelineState* pIPipelineState);
	VOID Uninitialize(VOID);

	ID3D12PipelineState* GetD3D12PipelineState(VOID);
	ID3D12RootSignature* GetD3D12RootSignature(VOID);
};

BOOL ReadShaderBytecode(const FILE_PATH& Path, SHADER_BYTECODE& rDesc);
VOID ReleaseShaderBytecode(SHADER_BYTECODE& rDesc);

#endif // WDK_CRENDERER_HPP
