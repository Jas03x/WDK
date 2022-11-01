
#include <Wdk.hpp>
#include <WdkGfx.hpp>
#include <WdkSystem.hpp>

#include <vector>

CONST PCWCHAR WINDOW_CLASS = L"HelloCube";
CONST PCWCHAR WINDOW_TITLE = L"Hello Cube";
CONST ULONG WINDOW_HEIGHT  = 512;
CONST ULONG WINDOW_WIDTH   = 512;

class HelloTriangle
{
private:
	IWindow*        m_pIWindow;
	IGfxDevice*     m_pIGfxDevice;
	IRenderer*      m_pIRenderer;
	IMesh*          m_pIMesh;
	ICommandBuffer* m_pIGraphicsCommandBuffer;

	CONST FLOAT     m_ClearColor[4] = { 0, 0, 0, 0 };

public:
	HelloTriangle()
	{
		m_pIWindow = NULL;
		m_pIGfxDevice = NULL;
		m_pIRenderer = NULL;
		m_pIMesh = NULL;
		m_pIGraphicsCommandBuffer = NULL;
	}

	BOOL Initialize(VOID)
	{
		BOOL Status = TRUE;
		std::vector<WCHAR> module_directory(1024);

		if (Status == TRUE)
		{
			m_pIWindow = WindowFactory::CreateInstance(WINDOW_CLASS, WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
			if (m_pIWindow == NULL)
			{
				Status = FALSE;
				Console::Write(L"Error: Could not initialize window\n");
			}
		}

		if (Status == TRUE)
		{
			DeviceFactory::Descriptor GfxDeviceDesc = {};
			GfxDeviceDesc.UploadHeapSize = static_cast<UINT64>(64U * MB);
			GfxDeviceDesc.PrimaryHeapSize = static_cast<UINT64>(64U * MB);

			m_pIGfxDevice = DeviceFactory::CreateInstance(m_pIWindow, GfxDeviceDesc);
			if (m_pIGfxDevice == NULL)
			{
				Status = FALSE;
				Console::Write(L"Error: Could not initialize graphics device\n");
			}
		}
		
		if (Status == TRUE)
		{
			Status = System::GetModuleDirectory(module_directory.data(), static_cast<DWORD>(module_directory.size()));
		}

		if (Status == TRUE)
		{
			RENDERER_DESC desc = {};

			INPUT_ELEMENT_DESC InputElements[] =
			{
				{ INPUT_ELEMENT_POSITION, 0, INPUT_ELEMENT_FORMAT_XYZ_32F, 0, sizeof(FLOAT) * 0, INPUT_ELEMENT_TYPE_PER_VERTEX, 0 },
				{ INPUT_ELEMENT_COLOR,    0, INPUT_ELEMENT_FORMAT_RGB_32F, 0, sizeof(FLOAT) * 3, INPUT_ELEMENT_TYPE_PER_VERTEX, 0 },
			};

			ReadShaderBytecode(FILE_PATH(module_directory.data(), L"/VertexShader.cso"), desc.VertexShader);
			ReadShaderBytecode(FILE_PATH(module_directory.data(), L"/PixelShader.cso"),  desc.PixelShader);

			desc.InputLayout.pInputElements = InputElements;
			desc.InputLayout.NumInputs = sizeof(InputElements) / sizeof(INPUT_ELEMENT_DESC);

			m_pIRenderer = m_pIGfxDevice->CreateRenderer(desc);
			if (m_pIRenderer == NULL)
			{
				Status = FALSE;
				Console::Write(L"Error: Could not initialize renderer\n");
			}

			ReleaseShaderBytecode(desc.VertexShader);
			ReleaseShaderBytecode(desc.PixelShader);
		}

		if (Status == TRUE)
		{
			m_pIGraphicsCommandBuffer = m_pIGfxDevice->CreateCommandBuffer(COMMAND_BUFFER_TYPE_GRAPHICS);

			if (m_pIGraphicsCommandBuffer == NULL)
			{
				Status = FALSE;
				Console::Write(L"Error: could not create graphics command buffer\n");
			}
		}

		if (Status == TRUE)
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

			static CONST FLOAT Vertices[][3] =
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

			static CONST FLOAT Colours[][3] =
			{
				{ 1.0, 0.0, 0.0 }, // Red
				{ 0.0, 1.0, 0.0 }, // Green
				{ 0.0, 0.0, 1.0 }, // Blue
				{ 1.0, 1.0, 0.0 }, // Yellow
				{ 1.0, 0.5, 0.0 }, // Orange
				{ 1.0, 1.0, 1.0 }  // White
			};

			static CONST Face Faces[] =
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
						4, 5, 6,
						5, 6, 7
					},
					1
				},

				// Right:
				{
					{
						1, 3, 7,
						1, 5, 7
					},
					2
				},

				// Left:
				{
					{
						0, 2, 4,
						0, 4, 6
					},
					3
				},

				// Top:
				{
					{
						0, 1, 4,
						1, 4, 5
					},
					4
				},

				// Bottom:
				{
					{
						2, 3, 6,
						3, 6, 7
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
			MeshDesc.BufferSize = sizeof(VertexArray);
			MeshDesc.Stride = sizeof(Vertex);
			MeshDesc.NumVertices = sizeof(VertexArray) / sizeof(Vertex);

			m_pIMesh = m_pIGfxDevice->CreateMesh(VertexArray, MeshDesc);

			if (m_pIMesh == NULL)
			{
				Status = FALSE;
				Console::Write(L"Error: could not create mesh\n");
			}
		}

		if (Status == FALSE)
		{
			Uninitialize();
		}

		return Status;
	}

	VOID Uninitialize(VOID)
	{
		if (m_pIMesh != NULL)
		{
			m_pIGfxDevice->DestroyMesh(m_pIMesh);
			m_pIMesh = NULL;
		}

		if (m_pIGraphicsCommandBuffer != NULL)
		{
			m_pIGfxDevice->DestroyCommandBuffer(m_pIGraphicsCommandBuffer);
			m_pIGraphicsCommandBuffer = NULL;
		}

		if (m_pIRenderer != NULL)
		{
			m_pIGfxDevice->DestroyRenderer(m_pIRenderer);
			m_pIRenderer = NULL;
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
	BOOL Render(VOID)
	{
		BOOL Status = TRUE;

		Status = m_pIGraphicsCommandBuffer->Reset();

		if (Status == TRUE)
		{
			RenderBuffer CurrentBuffer = m_pIWindow->GetCurrentRenderBuffer();

			m_pIGraphicsCommandBuffer->SetRenderer(m_pIRenderer);
			m_pIGraphicsCommandBuffer->SetViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, MIN_DEPTH, MAX_DEPTH);
			m_pIGraphicsCommandBuffer->SetRenderTarget(CurrentBuffer);
			m_pIGraphicsCommandBuffer->ClearRenderBuffer(CurrentBuffer, m_ClearColor);
			m_pIGraphicsCommandBuffer->Render(m_pIMesh);
			m_pIGraphicsCommandBuffer->Present(CurrentBuffer);

			Status = m_pIGraphicsCommandBuffer->Finalize();
		}

		if (Status == TRUE)
		{
			m_pIGfxDevice->SubmitCommandBuffer(m_pIGraphicsCommandBuffer);
		}

		if (Status == TRUE)
		{
			Status = m_pIWindow->Present();
		}

		if (Status == TRUE)
		{
			Status = m_pIGfxDevice->SyncQueue(COMMAND_QUEUE_TYPE_GRAPHICS);
		}

		return Status;
	}

public:
	BOOL Run()
	{
		BOOL Status = TRUE;
		WIN_EVENT WinEvent = {};

		while ((Status == TRUE) && (m_pIWindow->Open()))
		{
			if (m_pIWindow->GetEvent(WinEvent) == TRUE)
			{
				if (WinEvent.msg == WIN_MSG::QUIT)
				{
					break;
				}
			}
			else
			{
				Status = Render();
			}
		}

		return Status;
	}
};

INT WdkMain(INT argc, PWCHAR argv)
{
	INT Status = STATUS::SUCCESS;

	Console::Write(L"Hello Triangle!\n");

	HelloTriangle hello_triangle;

	if (hello_triangle.Initialize() != TRUE)
	{
		Status = STATUS::UNSUCCESSFUL;
	}

	if (Status == STATUS::SUCCESS)
	{
		if (hello_triangle.Run() == FALSE)
		{
			Status = STATUS::UNSUCCESSFUL;
		}
	}

	hello_triangle.Uninitialize();

	if (Status == STATUS::SUCCESS)
	{
		Console::Write(L"Goodbye Triangle!\n");
	}
	else
	{
		Console::Write(L"Exit with error 0x%X\n", Status);
	}
	
	return Status;
}
