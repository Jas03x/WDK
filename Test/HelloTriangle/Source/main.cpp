
#include <Wdk.hpp>
#include <WdkGfx.hpp>
#include <WdkSystem.hpp>

using namespace Wdk;

CONST PCWCHAR WINDOW_CLASS = L"HelloTriangle";
CONST PCWCHAR WINDOW_TITLE = L"Hello Triangle";
CONST ULONG WINDOW_HEIGHT = 512;
CONST ULONG WINDOW_WIDTH  = 512;

class HelloTriangle
{
private:
	IWindow*    m_pIWindow;
	IGfxDevice* m_pIGfxDevice;

public:
	HelloTriangle()
	{
		m_pIWindow = NULL;
		m_pIGfxDevice = NULL;
	}

	BOOL Initialize(VOID)
	{
		BOOL Status = TRUE;

		if (Status == TRUE)
		{
			m_pIWindow = CreateWindow(WINDOW_CLASS, WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
			if (m_pIWindow == NULL)
			{
				Status = FALSE;
				Console::Write(L"Error: Could not initialize window\n");
			}
		}

		if (Status == TRUE)
		{
			m_pIGfxDevice = CreateDevice(m_pIWindow);
			if (m_pIGfxDevice == NULL)
			{
				Status = FALSE;
				Console::Write(L"Error: Could not initialize graphics device\n");
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
		if (m_pIGfxDevice != NULL)
		{
			DestroyDevice(m_pIGfxDevice);
			m_pIGfxDevice = NULL;
		}

		if (m_pIWindow != NULL)
		{
			DestroyWindow(m_pIWindow);
			m_pIWindow = NULL;
		}
	}

	BOOL Run()
	{
		BOOL Status = TRUE;

		BOOL isRunning = TRUE;
		WinEvent winEvent = {};

		while ((Status == TRUE) && (isRunning == TRUE) && (m_pIWindow->Open()))
		{
			while (m_pIWindow->GetEvent(winEvent) == TRUE)
			{
				if (winEvent.msg == WIN_MSG::QUIT)
				{
					isRunning = FALSE;
				}
			}
		}

		return Status;
	}
};

INT WdkMain(INT argc, PWCHAR argv)
{
	INT Status = 0;

	Console::Write(L"Hello Triangle!\n");

	HelloTriangle hello_triangle;

	if (hello_triangle.Initialize() != TRUE)
	{
		Status = -1;
	}

	if (Status == 0)
	{
		Status = hello_triangle.Run();
	}

	hello_triangle.Uninitialize();

	if (Status == 0)
	{
		Console::Write(L"Goodbye Triangle!\n");
	}
	else
	{
		Console::Write(L"Exit with error %u\n", Status);
	}
	
	return Status;
}
