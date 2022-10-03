
#include <Wdk.hpp>
#include <WdkGfx.hpp>
#include <WdkSystem.hpp>

#include <vector>

CONST PCWCHAR WINDOW_CLASS = L"HelloTriangle";
CONST PCWCHAR WINDOW_TITLE = L"Hello Triangle";
CONST ULONG WINDOW_HEIGHT  = 512;
CONST ULONG WINDOW_WIDTH   = 512;

class HelloTriangle
{
private:
	IWindow*    m_pIWindow;
	IGfxDevice* m_pIGfxDevice;
	IRenderer*  m_pIRenderer;

public:
	HelloTriangle()
	{
		m_pIWindow = NULL;
		m_pIGfxDevice = NULL;
		m_pIRenderer = NULL;
	}

	BOOL Initialize(VOID)
	{
		BOOL Status = TRUE;
		std::vector<WCHAR> module_directory(1024);

		if (Status == TRUE)
		{
			m_pIWindow = IWindow::CreateInstance(WINDOW_CLASS, WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
			if (m_pIWindow == NULL)
			{
				Status = FALSE;
				Console::Write(L"Error: Could not initialize window\n");
			}
		}

		if (Status == TRUE)
		{
			IGfxDevice::Desc GfxDeviceDesc = {};
			GfxDeviceDesc.pIWindow = m_pIWindow;

			m_pIGfxDevice = IGfxDevice::CreateInstance(GfxDeviceDesc);
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
				{ INPUT_ELEMENT_POSITION, 0, INPUT_ELEMENT_FORMAT_XYZ_32F, 0, sizeof(FLOAT) * 0, INPUT_ELEMENT_TYPE_PER_VERTEX, 0},
				{ INPUT_ELEMENT_COLOR,    0, INPUT_ELEMENT_FORMAT_RGB_32F, 0, sizeof(FLOAT) * 3, INPUT_ELEMENT_TYPE_PER_VERTEX, 0},
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

		if (Status == FALSE)
		{
			Uninitialize();
		}

		return Status;
	}

	VOID Uninitialize(VOID)
	{
		if (m_pIRenderer != NULL)
		{
			m_pIGfxDevice->DestroyRenderer(m_pIRenderer);
			m_pIRenderer = NULL;
		}

		if (m_pIGfxDevice != NULL)
		{
			IGfxDevice::DestroyInstance(m_pIGfxDevice);
			m_pIGfxDevice = NULL;
		}

		if (m_pIWindow != NULL)
		{
			IWindow::DestroyInstance(m_pIWindow);
			m_pIWindow = NULL;
		}
	}

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
				// Render
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
