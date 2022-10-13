#include "CRenderer.hpp"

#include <d3d12.h>

#include "Wdk.hpp"

BOOL ReadShaderBytecode(const FILE_PATH& Path, SHADER_BYTECODE& rDesc)
{
	BOOL Status = TRUE;

	File* pFile = File::Open(Path);
	if (pFile == NULL)
	{
		Status = FALSE;
		Console::Write(L"Error: Failed to open shader file %s for reading\n", Path.FileName);
	}

	if (Status == TRUE)
	{
		if (pFile->Read(&rDesc.pCode, &rDesc.Size) != TRUE)
		{
			Status = FALSE;
			Console::Write(L"Error: Failed to read shader file %s\n", Path.FileName);
		}
	}

	if (pFile != NULL)
	{
		File::Close(pFile);
		pFile = NULL;
	}

	return Status;
}

VOID ReleaseShaderBytecode(SHADER_BYTECODE& rDesc)
{
	if (rDesc.pCode != NULL)
	{
		Memory::Release(rDesc.pCode);
		rDesc.pCode = NULL;
	}

	rDesc.Size = 0;
}

CRenderer::CRenderer(VOID)
{
	m_pID3D12PipelineState = NULL;
}

CRenderer::~CRenderer(VOID)
{
}

BOOL CRenderer::Initialize(ID3D12PipelineState* pIPipelineState)
{
	BOOL Status = TRUE;

	if (pIPipelineState != NULL)
	{
		m_pID3D12PipelineState = pIPipelineState;
	}
	else
	{
		Status = FALSE;
		Console::Write(L"Error: Invalid pointer\n");
	}

	return Status;
}

VOID CRenderer::Uninitialize(VOID)
{
	if (m_pID3D12PipelineState != NULL)
	{
		m_pID3D12PipelineState->Release();
		m_pID3D12PipelineState = NULL;
	}
}
