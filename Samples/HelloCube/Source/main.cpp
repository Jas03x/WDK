
#include <Wdk.hpp>
#include <WdkGfx.hpp>
#include "WdkMath.hpp"
#include <WdkSystem.hpp>

#include "Windows.h"

#include <vector>

const PCWCHAR WINDOW_CLASS = L"HelloCube";
const PCWCHAR WINDOW_TITLE = L"Hello Cube";
const ULONG WINDOW_HEIGHT  = 512;
const ULONG WINDOW_WIDTH   = 512;

class HelloCube
{
private:
	union ConstantBuffer
	{
		struct
		{
			Matrix4F Transform;
		};

		BYTE Padding[256]; // needs to be aligned to 256 bytes

		ConstantBuffer()
		{
			ZeroMemory(this, sizeof(ConstantBuffer));

			Transform = Matrix4F(1.0);
		}
	} m_ConstantBuffer;

	IWindow*         m_pIWindow;
	IGfxDevice*      m_pIGfxDevice;

	IConstantBuffer* m_pIConstantBuffer;
	IRendererState*  m_pIRendererState;
	ICommandBuffer*  m_pIGraphicsCommandBuffer;

	IMesh*           m_pIMesh;

	const float      m_ClearColor[4] = { 0, 0, 0, 0 };

public:
	HelloCube()
	{
		m_pIWindow = NULL;
		m_pIGfxDevice = NULL;
		m_pIRendererState = NULL;
		m_pIMesh = NULL;
		m_pIGraphicsCommandBuffer = NULL;
		m_pIConstantBuffer = NULL;
	}

	bool Initialize(void)
	{
		bool status = true;
		std::vector<WCHAR> module_directory(1024);

		if (status)
		{
			m_pIWindow = WindowFactory::CreateInstance(WINDOW_CLASS, WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
			if (m_pIWindow == NULL)
			{
				status = false;
				Console::Write(L"Error: Could not initialize window\n");
			}
		}

		if (status)
		{
			DeviceFactory::Descriptor GfxDeviceDesc = {};
			GfxDeviceDesc.UploadHeapSize = static_cast<UINT64>(64U * MB);
			GfxDeviceDesc.PrimaryHeapSize = static_cast<UINT64>(64U * MB);

			m_pIGfxDevice = DeviceFactory::CreateInstance(m_pIWindow, GfxDeviceDesc);
			if (m_pIGfxDevice == NULL)
			{
				status = false;
				Console::Write(L"Error: Could not initialize graphics device\n");
			}
		}
		
		if (status)
		{
			status = System::GetModuleDirectory(module_directory.data(), static_cast<DWORD>(module_directory.size()));
		}

		if (status)
		{
			RENDERER_STATE_DESC Desc = {};
			Desc.EnableDepthClipping = false;

			INPUT_ELEMENT_DESC InputElements[] =
			{
				{ INPUT_ELEMENT_POSITION, 0, INPUT_ELEMENT_FORMAT_XYZ_32F, 0, sizeof(float) * 0, INPUT_ELEMENT_TYPE_PER_VERTEX, 0 },
				{ INPUT_ELEMENT_COLOR,    0, INPUT_ELEMENT_FORMAT_RGB_32F, 0, sizeof(float) * 3, INPUT_ELEMENT_TYPE_PER_VERTEX, 0 },
			};

			ReadShaderBytecode(FILE_PATH(module_directory.data(), L"/VertexShader.cso"), Desc.VertexShader);
			ReadShaderBytecode(FILE_PATH(module_directory.data(), L"/PixelShader.cso"),  Desc.PixelShader);

			Desc.InputLayout.pInputElements = InputElements;
			Desc.InputLayout.NumInputs = sizeof(InputElements) / sizeof(INPUT_ELEMENT_DESC);

			m_pIRendererState = m_pIGfxDevice->CreateRendererState(Desc);
			if (m_pIRendererState == NULL)
			{
				status = false;
				Console::Write(L"Error: Could not initialize renderer\n");
			}

			ReleaseShaderBytecode(Desc.VertexShader);
			ReleaseShaderBytecode(Desc.PixelShader);
		}

		if (status)
		{
			m_pIGraphicsCommandBuffer = m_pIGfxDevice->CreateCommandBuffer(COMMAND_BUFFER_TYPE_GRAPHICS);

			if (m_pIGraphicsCommandBuffer == NULL)
			{
				status = false;
				Console::Write(L"Error: could not create graphics command buffer\n");
			}
		}

		if (status)
		{
			CONSTANT_BUFFER_DESC Desc = {};
			Desc.Size = sizeof(ConstantBuffer);

			m_pIConstantBuffer = m_pIGfxDevice->CreateConstantBuffer(Desc);

			if (m_pIConstantBuffer == NULL)
			{
				status = false;
				Console::Write(L"Error: could not create constant buffer\n");
			}
		}

		if (status)
		{
			/*
			*      4 ______________ 5
			*       /|            /|
			*      / |           / |
			*   0 /__|__________/1 |
			*     |  |          |  |
			*     |  |__________|__|
			*     |  /6         | 7/
			*     | /           | /
			*     |/____________|/
			*   2                3
			* 
			* 0: (-0.5, +0.5, +0.5)
			* 1: (+0.5, +0.5, +0.5)
			* 2: (-0.5, -0.5, +0.5)
			* 3: (+0.5, -0.5, +0.5)
			* 4: (-0.5, +0.5, -0.5)
			* 5: (+0.5, +0.5, -0.5)
			* 6: (-0.5, -0.5, -0.5)
			* 7: (+0.5, -0.5, -0.5)
			*/

			struct Vertex
			{
				float vertex[3];
				float colour[3];
			};

			struct Face
			{
				UINT vertices[6];
				UINT colour;
			};

			static const float Vertices[][3] =
			{
				{ -0.5, +0.5, +0.5 },
				{ +0.5, +0.5, +0.5 },
				{ -0.5, -0.5, +0.5 },
				{ +0.5, -0.5, +0.5 },
				{ -0.5, +0.5, -0.5 },
				{ +0.5, +0.5, -0.5 },
				{ -0.5, -0.5, -0.5 },
				{ +0.5, -0.5, -0.5 }
			};

			static const float Colours[][3] =
			{
				{ 1.0, 0.0, 0.0 }, // Red
				{ 0.0, 1.0, 0.0 }, // Green
				{ 0.0, 0.0, 1.0 }, // Blue
				{ 1.0, 1.0, 0.0 }, // Yellow
				{ 1.0, 0.5, 0.0 }, // Orange
				{ 1.0, 1.0, 1.0 }  // White
			};

			static const Face Faces[] =
			{
				// Front:
				{
					{
						0, 1, 2,
						3, 2, 1
					},
					0
				},

				// Back:
				{
					{
						6, 5, 4,
						5, 6, 7
					},
					1
				},

				// Right:
				{
					{
						7, 3, 1,
						1, 5, 7
					},
					2
				},

				// Left:
				{
					{
						0, 2, 4,
						6, 4, 2
					},
					3
				},

				// Top:
				{
					{
						4, 1, 0,
						1, 4, 5
					},
					4
				},

				// Bottom:
				{
					{
						2, 3, 6,
						7, 6, 3
					},
					5
				}
			};

			Vertex VertexArray[_countof(Faces) * 6] = {};

			for (UINT i = 0; i < _countof(Faces); i++)
			{
				for (UINT j = 0; j < 6; j++)
				{
					Vertex& v = VertexArray[i * _countof(Faces) + j];

					v.vertex[0] = Vertices[Faces[i].vertices[j]][0];
					v.vertex[1] = Vertices[Faces[i].vertices[j]][1];
					v.vertex[2] = Vertices[Faces[i].vertices[j]][2];
					v.colour[0] = Colours[Faces[i].colour][0];
					v.colour[1] = Colours[Faces[i].colour][1];
					v.colour[2] = Colours[Faces[i].colour][2];
				}
			}

			MESH_DESC MeshDesc = {};
			MeshDesc.pVertexData = VertexArray;
			MeshDesc.VertexBufferSize = sizeof(VertexArray);
			MeshDesc.VertexStride = sizeof(Vertex);
			MeshDesc.NumVertices = sizeof(VertexArray) / sizeof(Vertex);

			m_pIMesh = m_pIGfxDevice->CreateMesh(MeshDesc);

			if (m_pIMesh == NULL)
			{
				status = false;
				Console::Write(L"Error: could not create mesh\n");
			}
		}

		if (!status)
		{
			Uninitialize();
		}

		return status;
	}

	void Uninitialize(void)
	{
		if (m_pIMesh != NULL)
		{
			m_pIGfxDevice->DestroyMesh(m_pIMesh);
			m_pIMesh = NULL;
		}

		if (m_pIConstantBuffer != NULL)
		{
			m_pIGfxDevice->DestroyConstantBuffer(m_pIConstantBuffer);
			m_pIConstantBuffer = NULL;
		}

		if (m_pIGraphicsCommandBuffer != NULL)
		{
			m_pIGfxDevice->DestroyCommandBuffer(m_pIGraphicsCommandBuffer);
			m_pIGraphicsCommandBuffer = NULL;
		}

		if (m_pIRendererState != NULL)
		{
			m_pIGfxDevice->DestroyRendererState(m_pIRendererState);
			m_pIRendererState = NULL;
		}

		if (m_pIGfxDevice != NULL)
		{
			DeviceFactory::DestroyInstance(m_pIGfxDevice);
			m_pIGfxDevice = NULL;
		}

		if (m_pIWindow != NULL)
		{
			WindowFactory::DestroyInstance(m_pIWindow);
			m_pIWindow = NULL;
		}
	}

private:
	bool Update(void)
	{
		bool status = true;

		m_ConstantBuffer.Transform *= Matrix4F(Quaternion(0.01f, 0.01f, 0.0f));
		memcpy(m_pIConstantBuffer->GetCpuVA(), &m_ConstantBuffer, sizeof(ConstantBuffer));

		return status;
	}

	bool Render(void)
	{
		bool status = true;

		status = m_pIGraphicsCommandBuffer->Reset();

		if (status)
		{
			RenderBuffer CurrentBuffer = m_pIWindow->GetCurrentRenderBuffer();

			m_pIGraphicsCommandBuffer->SetViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, MIN_DEPTH, MAX_DEPTH);
			m_pIGraphicsCommandBuffer->SetRenderTarget(CurrentBuffer);
			m_pIGraphicsCommandBuffer->ClearRenderBuffer(CurrentBuffer, m_ClearColor);

			m_pIGraphicsCommandBuffer->ProgramPipeline(m_pIRendererState);
			m_pIGraphicsCommandBuffer->SetConstantBuffer(0, m_pIConstantBuffer);
			m_pIGraphicsCommandBuffer->SetVertexBuffers(1, m_pIMesh->GetVertexBuffer());
			m_pIGraphicsCommandBuffer->Draw(m_pIMesh->GetVertexCount());
			m_pIGraphicsCommandBuffer->Present(CurrentBuffer);

			status = m_pIGraphicsCommandBuffer->Finalize();
		}

		if (status)
		{
			m_pIGfxDevice->SubmitCommandBuffer(m_pIGraphicsCommandBuffer);
		}

		if (status)
		{
			status = m_pIWindow->Present();
		}

		if (status)
		{
			status = m_pIGfxDevice->SyncQueue(COMMAND_QUEUE_TYPE_GRAPHICS);
		}

		return status;
	}

public:
	bool Run()
	{
		bool status = true;
		WIN_EVENT WinEvent = {};

		while (status && (m_pIWindow->Open()))
		{
			if (m_pIWindow->GetEvent(WinEvent) == true)
			{
				if (WinEvent.msg == WIN_MSG::QUIT)
				{
					break;
				}
			}
			else
			{
				status = Update();

				if (status)
				{
					status = Render();
				}
			}
		}

		return status;
	}
};

int32_t WdkMain(int32_t argc, const wchar_t* argv[])
{
	int32_t status = STATUS::SUCCESS;

	Console::Write(L"Hello Cube!\n");

	HelloCube hello_cube;

	if (!hello_cube.Initialize())
	{
		status = STATUS::UNSUCCESSFUL;
	}

	if (status == STATUS::SUCCESS)
	{
		if (!hello_cube.Run())
		{
			status = STATUS::UNSUCCESSFUL;
		}
	}

	hello_cube.Uninitialize();

	if (status == STATUS::SUCCESS)
	{
		Console::Write(L"Goodbye Triangle!\n");
	}
	else
	{
		Console::Write(L"Exit with error 0x%X\n", status);
	}
	
	return status;
}
