#include "CGfxDevice.hpp"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>

#include <math.h>

#include "WdkSystem.hpp"

#include "CCommandList.hpp"
#include "CFence.hpp"
#include "CRenderer.hpp"

IGfxDevice* IGfxDevice::CreateInstance(IGfxDevice::Descriptor& rDesc)
{
	return CGfxDevice::CreateInstance(rDesc);
}

VOID IGfxDevice::DestroyInstance(IGfxDevice* pIDevice)
{
	CGfxDevice::DestroyInstance(static_cast<CGfxDevice*>(pIDevice));
}

CGfxDevice* CGfxDevice::CreateInstance(IGfxDevice::Descriptor& rDesc)
{
	CGfxDevice* pDevice = new CGfxDevice();

	if (pDevice != NULL)
	{
		if (pDevice->Initialize(rDesc) == FALSE)
		{
			DestroyInstance(pDevice);
			pDevice = NULL;
		}
	}

	return pDevice;
}

VOID CGfxDevice::DestroyInstance(CGfxDevice* pDevice)
{
	if (pDevice != NULL)
	{
		pDevice->Uninitialize();

		delete pDevice;
		pDevice = NULL;
	}
}

CGfxDevice::CGfxDevice(VOID)
{
	m_pIWindow = NULL;

	m_hDxgiDebugModule = NULL;

	m_pIDxgiDebugInterface = NULL;
	m_pID3D12DebugInterface = NULL;

	m_pIDxgiFactory = NULL;
	m_pIDxgiAdapter = NULL;
	m_pIDxgiSwapChain = NULL;

	m_pID3D12Device = NULL;
	m_pID3D12CommandQueue = NULL;
	m_pID3D12RtvDescriptorHeap = NULL;
	m_pID3D12CommandAllocator = NULL;
	m_pID3D12RootSignature = NULL;

	for (UINT i = 0; i < NUM_BUFFERS; i++)
	{
		m_pID3D12RenderBuffers[i] = NULL;
	}

	m_FrameIndex = 0;
	m_RtvDescriptorIncrement = 0;
}

CGfxDevice::~CGfxDevice(VOID)
{

}

BOOL CGfxDevice::Initialize(IGfxDevice::Descriptor& rDesc)
{
	BOOL Status = TRUE;

	m_pIWindow = rDesc.pIWindow;

#if _DEBUG
	if (D3D12GetDebugInterface(__uuidof(ID3D12Debug), reinterpret_cast<VOID**>(&m_pID3D12DebugInterface)) == S_OK)
	{
		m_pID3D12DebugInterface->EnableDebugLayer();
	}
	else
	{
		Status = FALSE;
		Console::Write(L"Error: Failed to get dx12 debug interface\n");
	}

	if (Status == TRUE)
	{
		m_hDxgiDebugModule = GetModuleHandle(L"dxgidebug.dll");
		HRESULT (*pfnDxgiGetDebugInterface)(REFIID, VOID**) = NULL;

		if (m_hDxgiDebugModule != NULL)
		{
			pfnDxgiGetDebugInterface = reinterpret_cast<HRESULT(*)(REFIID, VOID**)>(GetProcAddress(m_hDxgiDebugModule, "DXGIGetDebugInterface"));

			if (pfnDxgiGetDebugInterface == NULL)
			{
				Console::Write(L"Error: Could not find function DXGIGetDebugInterface in the dxgi debug module\n");
				Status = FALSE;
			}
		}
		else
		{
			Console::Write(L"Error: Could not load the dxgi debug module\n");
			Status = FALSE;
		}

		if (Status == TRUE)
		{
			if (pfnDxgiGetDebugInterface(__uuidof(IDXGIDebug), reinterpret_cast<VOID**>(&m_pIDxgiDebugInterface)) != S_OK)
			{
				Console::Write(L"Error: Failed to get dxgi debug interface\n");
				Status = FALSE;
			}
		}
	}
#endif

	if (Status == TRUE)
	{
		UINT Flags = 0;

#if _DEBUG
		Flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

		if (CreateDXGIFactory2(Flags, __uuidof(IDXGIFactory7), reinterpret_cast<VOID**>(&m_pIDxgiFactory)) != S_OK)
		{
			Status = FALSE;
			Console::Write(L"Error: Failed to create dxgi factory\n");
		}
	}

	if (Status == TRUE)
	{
		Status = EnumerateDxgiAdapters();

		if (Status == FALSE)
		{
			Status = FALSE;
			Console::Write(L"Error: Could not get dxgi adapter\n");
		}
	}

	if (Status == TRUE)
	{
		D3D12CreateDevice(m_pIDxgiAdapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), reinterpret_cast<VOID**>(&m_pID3D12Device));

		if (m_pID3D12Device != NULL)
		{
			PrintDeviceProperties();
		}
		else
		{
			Status = FALSE;
			Console::Write(L"Error: Could not create a DX12 device\n");
		}
	}

	if (Status == TRUE)
	{
		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = { };
		cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		cmdQueueDesc.NodeMask = 0;

		if (m_pID3D12Device->CreateCommandQueue(&cmdQueueDesc, __uuidof(ID3D12CommandQueue), reinterpret_cast<VOID**>(&m_pID3D12CommandQueue)) != S_OK)
		{
			Status = FALSE;
			Console::Write(L"Error: Failed to create command queue\n");
		}
	}

	if (Status == TRUE)
	{
		WIN_RECT rect = {};
		
		if (m_pIWindow->GetRect(WIN_AREA::CLIENT, rect) == TRUE)
		{
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { };
			swapChainDesc.Width = rect.width;
			swapChainDesc.Height = rect.height;
			swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.Stereo = FALSE;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = NUM_BUFFERS;
			swapChainDesc.Scaling = DXGI_SCALING_NONE;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			swapChainDesc.Flags = 0;

			IDXGISwapChain1* pISwapChain1 = NULL;

			if (m_pIDxgiFactory->CreateSwapChainForHwnd(m_pID3D12CommandQueue, m_pIWindow->GetHandle(), &swapChainDesc, NULL, NULL, &pISwapChain1) == S_OK)
			{
				pISwapChain1->QueryInterface(__uuidof(IDXGISwapChain4), reinterpret_cast<VOID**>(&m_pIDxgiSwapChain));
				pISwapChain1->Release();

				m_FrameIndex = m_pIDxgiSwapChain->GetCurrentBackBufferIndex();
			}
			else
			{
				Status = FALSE;
				Console::Write(L"Error: Failed to create swap chain\n");
			}
		}
		else
		{
			Status = FALSE;
		}
	}

	if (Status == TRUE)
	{
		D3D12_DESCRIPTOR_HEAP_DESC descHeap = {};
		descHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		descHeap.NumDescriptors = NUM_BUFFERS;
		descHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		descHeap.NodeMask = 0;

		if (m_pID3D12Device->CreateDescriptorHeap(&descHeap, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<VOID**>(&m_pID3D12RtvDescriptorHeap)) == S_OK)
		{
			m_RtvDescriptorIncrement = m_pID3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		}
		else
		{
			Console::Write(L"Error: Failed to create descriptor heap\n");
			Status = FALSE;
		}
	}

	if (Status == TRUE)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandle = m_pID3D12RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		for (UINT i = 0; (Status == TRUE) && (i < NUM_BUFFERS); i++)
		{
			if (m_pIDxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), reinterpret_cast<VOID**>(&m_pID3D12RenderBuffers[i])) != S_OK)
			{
				Console::Write(L"Error: Could not get swap chain buffer %u\n", i);
				Status = FALSE;
			}

			m_pID3D12Device->CreateRenderTargetView(m_pID3D12RenderBuffers[i], NULL, cpuDescHandle);

			cpuDescHandle.ptr += m_RtvDescriptorIncrement;
		}
	}

	if (Status == TRUE)
	{
		if (m_pID3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), reinterpret_cast<VOID**>(&m_pID3D12CommandAllocator)) != S_OK)
		{
			Console::Write(L"Error: Could not create command allocator\n");
			Status = FALSE;
		}
	}

	if (Status == TRUE)
	{
		D3D12_ROOT_SIGNATURE_DESC desc = { };
		desc.NumParameters = 0;
		desc.pParameters = NULL;
		desc.NumStaticSamplers = 0;
		desc.pStaticSamplers = NULL;
		desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ID3DBlob* pSignature = NULL;
		ID3DBlob* pError = NULL;

		if (D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &pSignature, &pError) == S_OK)
		{
			if (m_pID3D12Device->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), __uuidof(ID3D12RootSignature), reinterpret_cast<VOID**>(&m_pID3D12RootSignature)) != S_OK)
			{
				Console::Write(L"Error: Could not create root signature\n");
				Status = FALSE;
			}
		}
		else
		{
			Console::Write(L"Error: Could not initialize root signature\n");
			Status = FALSE;

			if (pError != NULL)
			{
				Console::Write(L"Error Info: %s\n", pError->GetBufferPointer());
			}
		}

		if (pSignature != NULL)
		{
			pSignature->Release();
			pSignature = NULL;
		}

		if (pError != NULL)
		{
			pError->Release();
			pError = NULL;
		}
	}

	if (Status == TRUE)
	{
		D3D12_RESOURCE_DESC UploadHeapResourceDesc = { };
		UploadHeapResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		UploadHeapResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		UploadHeapResourceDesc.Width = rDesc.UploadHeapSize;
		UploadHeapResourceDesc.Height = 1;
		UploadHeapResourceDesc.DepthOrArraySize = 1;
		UploadHeapResourceDesc.MipLevels = 1;
		UploadHeapResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		UploadHeapResourceDesc.SampleDesc.Count = 1;
		UploadHeapResourceDesc.SampleDesc.Quality = 0;
		UploadHeapResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		UploadHeapResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12_RESOURCE_ALLOCATION_INFO UploadHeapAllocationInfo = m_pID3D12Device->GetResourceAllocationInfo(0, 1, &UploadHeapResourceDesc);

		D3D12_HEAP_DESC UploadHeapDesc = { };
		UploadHeapDesc.SizeInBytes = UploadHeapAllocationInfo.SizeInBytes;
		UploadHeapDesc.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;
		UploadHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		UploadHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		UploadHeapDesc.Properties.CreationNodeMask = 1;
		UploadHeapDesc.Properties.VisibleNodeMask = 1;

		if (m_pID3D12Device->CreateHeap(&UploadHeapDesc, __uuidof(ID3D12Heap), reinterpret_cast<VOID**>(&m_pID3D12UploadHeap)) != S_OK)
		{
			Status = FALSE;
			Console::Write(L"Error: Failed to create upload heap\n");
		}
	}

	if (Status == TRUE)
	{
		D3D12_RESOURCE_DESC PrimaryHeapResourceDesc = { };
		PrimaryHeapResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		PrimaryHeapResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		PrimaryHeapResourceDesc.Width = rDesc.PrimaryHeapSize;
		PrimaryHeapResourceDesc.Height = 1;
		PrimaryHeapResourceDesc.DepthOrArraySize = 1;
		PrimaryHeapResourceDesc.MipLevels = 1;
		PrimaryHeapResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		PrimaryHeapResourceDesc.SampleDesc.Count = 1;
		PrimaryHeapResourceDesc.SampleDesc.Quality = 0;
		PrimaryHeapResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		PrimaryHeapResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12_RESOURCE_ALLOCATION_INFO PrimaryHeapAllocationInfo = m_pID3D12Device->GetResourceAllocationInfo(0, 1, &PrimaryHeapResourceDesc);

		D3D12_HEAP_DESC PrimaryHeapDesc = { };
		PrimaryHeapDesc.SizeInBytes = PrimaryHeapAllocationInfo.SizeInBytes;
		PrimaryHeapDesc.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;
		PrimaryHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		PrimaryHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		PrimaryHeapDesc.Properties.CreationNodeMask = 1;
		PrimaryHeapDesc.Properties.VisibleNodeMask = 1;

		if (m_pID3D12Device->CreateHeap(&PrimaryHeapDesc, __uuidof(ID3D12Heap), reinterpret_cast<VOID**>(&m_pID3D12PrimaryHeap)) != S_OK)
		{
			Status = FALSE;
			Console::Write(L"Error: Failed to create upload heap\n");
		}
	}

	return Status;
}

VOID CGfxDevice::Uninitialize(VOID)
{
	if (m_pID3D12PrimaryHeap != NULL)
	{
		m_pID3D12PrimaryHeap->Release();
		m_pID3D12PrimaryHeap = NULL;
	}

	if (m_pID3D12UploadHeap != NULL)
	{
		m_pID3D12UploadHeap->Release();
		m_pID3D12UploadHeap = NULL;
	}

	if (m_pID3D12RootSignature != NULL)
	{
		m_pID3D12RootSignature->Release();
		m_pID3D12RootSignature = NULL;
	}

	if (m_pID3D12CommandAllocator != NULL)
	{
		m_pID3D12CommandAllocator->Release();
		m_pID3D12CommandAllocator = NULL;
	}

	for (UINT i = 0; i < NUM_BUFFERS; i++)
	{
		if (m_pID3D12RenderBuffers[i] != NULL)
		{
			m_pID3D12RenderBuffers[i]->Release();
			m_pID3D12RenderBuffers[i] = NULL;
		}
	}

	if (m_pID3D12RtvDescriptorHeap != NULL)
	{
		m_pID3D12RtvDescriptorHeap->Release();
		m_pID3D12RtvDescriptorHeap = NULL;
	}

	if (m_pIDxgiSwapChain != NULL)
	{
		m_pIDxgiSwapChain->Release();
		m_pIDxgiSwapChain = NULL;
	}

	if (m_pID3D12CommandQueue != NULL)
	{
		m_pID3D12CommandQueue->Release();
		m_pID3D12CommandQueue = NULL;
	}

	if (m_pID3D12Device != NULL)
	{
		m_pID3D12Device->Release();
		m_pID3D12Device = NULL;
	}

	if (m_pIDxgiAdapter != NULL)
	{
		m_pIDxgiAdapter->Release();
		m_pIDxgiAdapter = NULL;
	}

	if (m_pIDxgiFactory != NULL)
	{
		m_pIDxgiFactory->Release();
		m_pIDxgiFactory = NULL;
	}

#if _DEBUG
	if (m_pIDxgiDebugInterface != NULL)
	{
		m_pIDxgiDebugInterface->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);

		m_pIDxgiDebugInterface->Release();
		m_pIDxgiDebugInterface = NULL;
	}

	if (m_hDxgiDebugModule != NULL)
	{
		FreeLibrary(m_hDxgiDebugModule);
		m_hDxgiDebugModule = NULL;
	}

	if (m_pID3D12DebugInterface != NULL)
	{
		m_pID3D12DebugInterface->Release();
		m_pID3D12DebugInterface = NULL;
	}
#endif

	m_pIWindow = NULL;
}

BOOL CGfxDevice::EnumerateDxgiAdapters(VOID)
{
	BOOL Status = TRUE;
	UINT uIndex = 0;
	IDXGIAdapter4* pIAdapter = NULL;
	INT AdapterIndex = -1;

	while (Status == TRUE)
	{
		HRESULT result = m_pIDxgiFactory->EnumAdapterByGpuPreference(uIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, __uuidof(IDXGIAdapter4), reinterpret_cast<VOID**>(&pIAdapter));

		if (result == DXGI_ERROR_NOT_FOUND)
		{
			break;
		}
		else if (result != S_OK)
		{
			Console::Write(L"Error: Could not enumerate adapters\n");
			Status = FALSE;
			break;
		}
		else
		{
			Status = PrintAdapterProperties(uIndex, pIAdapter);

			if (Status == TRUE)
			{
				if (AdapterIndex == -1)
				{
					// Check if the adapter supports D3D12 - the result is S_FALSE if the function succeeds but ppDevice is NULL
					if (D3D12CreateDevice(pIAdapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), NULL) == S_FALSE)
					{
						AdapterIndex = uIndex;
						m_pIDxgiAdapter = pIAdapter;
					}
				}
			}

			if (pIAdapter != m_pIDxgiAdapter)
			{
				pIAdapter->Release();
			}

			uIndex++;
		}
	}

	if (m_pIDxgiAdapter != NULL)
	{
		Console::Write(L"Using adapter %u\n", AdapterIndex);
	}
	else
	{
		Console::Write(L"Error: Could not find adapter to use\n");
	}

	return Status;
}

BOOL CGfxDevice::PrintAdapterProperties(UINT uIndex, IDXGIAdapter4* pIAdapter)
{
	BOOL Status = TRUE;
	DXGI_ADAPTER_DESC3 Desc = { 0 };

	if (pIAdapter->GetDesc3(&Desc) == S_OK)
	{
		Console::Write(L"Adapter %u:\n", uIndex);
		Console::Write(L"\tDescription: %s\n", Desc.Description);
		Console::Write(L"\tVendorId: %X\n", Desc.VendorId);
		Console::Write(L"\tDeviceId: %X\n", Desc.DeviceId);
		Console::Write(L"\tsubSysId: %X\n", Desc.SubSysId);
		Console::Write(L"\tRevision: %X\n", Desc.Revision);
		Console::Write(L"\tDedicatedVideoMemory: %.0f GB\n", ceilf(static_cast<float>(Desc.DedicatedVideoMemory) / GB));
		Console::Write(L"\tDedicatedSystemMemory: %.0f GB\n", ceilf(static_cast<float>(Desc.DedicatedSystemMemory) / GB));
		Console::Write(L"\tSharedSystemMemory: %.0f GB\n", ceilf(static_cast<float>(Desc.SharedSystemMemory) / GB));

		if (Desc.Flags != DXGI_ADAPTER_FLAG3_NONE)
		{
			Console::Write(L"\tFlags:\n");

			if ((Desc.Flags & DXGI_ADAPTER_FLAG3_REMOTE) != 0)
			{
				Console::Write(L"\t\tRemote\n");
			}

			if ((Desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) != 0)
			{
				Console::Write(L"\t\tSoftware\n");
			}

			if ((Desc.Flags & DXGI_ADAPTER_FLAG3_ACG_COMPATIBLE) != 0)
			{
				Console::Write(L"\t\tACG_Compatible\n");
			}

			if ((Desc.Flags & DXGI_ADAPTER_FLAG3_SUPPORT_MONITORED_FENCES) != 0)
			{
				Console::Write(L"\t\tSupport_Monitored_Fences\n");
			}

			if ((Desc.Flags & DXGI_ADAPTER_FLAG3_SUPPORT_NON_MONITORED_FENCES) != 0)
			{
				Console::Write(L"\t\tSupport_Non_Monitored_Fences\n");
			}

			if ((Desc.Flags & DXGI_ADAPTER_FLAG3_KEYED_MUTEX_CONFORMANCE) != 0)
			{
				Console::Write(L"\t\tKeyed_Mutex_Conformance\n");
			}
		}

		LPCWCH GraphicsPreemptionGranularity = L"Unknown";
		LPCWCH ComputePreemptionGranularity = L"Unknown";

		switch (Desc.GraphicsPreemptionGranularity)
		{
		case DXGI_GRAPHICS_PREEMPTION_DMA_BUFFER_BOUNDARY:
			GraphicsPreemptionGranularity = L"Dma_Buffer_Boundary";
			break;
		case DXGI_GRAPHICS_PREEMPTION_PRIMITIVE_BOUNDARY:
			GraphicsPreemptionGranularity = L"Primitive_Boundary";
			break;
		case DXGI_GRAPHICS_PREEMPTION_TRIANGLE_BOUNDARY:
			GraphicsPreemptionGranularity = L"Triangle_Boundary";
			break;
		case DXGI_GRAPHICS_PREEMPTION_PIXEL_BOUNDARY:
			GraphicsPreemptionGranularity = L"Pixel_Boundary";
			break;
		case DXGI_GRAPHICS_PREEMPTION_INSTRUCTION_BOUNDARY:
			GraphicsPreemptionGranularity = L"Instruction_Boundary";
			break;
		}

		switch (Desc.ComputePreemptionGranularity)
		{
		case DXGI_COMPUTE_PREEMPTION_DMA_BUFFER_BOUNDARY:
			ComputePreemptionGranularity = L"Dma_Buffer_Boundary";
			break;
		case DXGI_COMPUTE_PREEMPTION_DISPATCH_BOUNDARY:
			ComputePreemptionGranularity = L"Dispatch_Boundary";
			break;
		case DXGI_COMPUTE_PREEMPTION_THREAD_GROUP_BOUNDARY:
			ComputePreemptionGranularity = L"Thread_Group_Boundary";
			break;
		case DXGI_COMPUTE_PREEMPTION_THREAD_BOUNDARY:
			ComputePreemptionGranularity = L"Thread_Boundary";
			break;
		case DXGI_COMPUTE_PREEMPTION_INSTRUCTION_BOUNDARY:
			ComputePreemptionGranularity = L"Instruction_Boundary";
			break;
		}

		Console::Write(L"\tGraphicsPreemptionBoundary: %s\n", GraphicsPreemptionGranularity);
		Console::Write(L"\tComputePreemptionGranularity: %s\n", ComputePreemptionGranularity);
	}
	else
	{
		Status = FALSE;
		Console::Write(L"Error: Could not get description for adapter %u\n", uIndex);
	}

	return Status;
}

BOOL CGfxDevice::PrintDeviceProperties(VOID)
{
	BOOL Status = TRUE;

	if (Status == TRUE)
	{
		D3D12_FEATURE_DATA_ARCHITECTURE1 ArchData = {};
		ArchData.NodeIndex = 0;

		if (m_pID3D12Device->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE1, &ArchData, sizeof(D3D12_FEATURE_DATA_ARCHITECTURE1)) == S_OK)
		{
			Console::Write(L"DX12 Device Properties:\n");
			Console::Write(L"\tTileBasedRenderer: %s\n", ArchData.TileBasedRenderer ? L"TRUE" : L"FALSE");
			Console::Write(L"\tUMA: %s\n", ArchData.UMA ? L"TRUE" : L"FALSE");
			Console::Write(L"\tCacheCoherentUMA: %s\n", ArchData.CacheCoherentUMA ? L"TRUE" : L"FALSE");
			Console::Write(L"\tIsolatedMMU: %s\n", ArchData.IsolatedMMU ? L"TRUE" : L"FALSE");
		}
		else
		{
			Status = FALSE;
			Console::Write(L"Error: Could not get device architecture\n");
		}
	}

	if (Status == TRUE)
	{
		DXGI_QUERY_VIDEO_MEMORY_INFO NonLocalMemoryInfo = {};

		if (m_pIDxgiAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, &NonLocalMemoryInfo) == S_OK)
		{
			Console::Write(L"\tNon-Local Memory Info:\n");
			Console::Write(L"\t\tBudget: %.2f GB\n", static_cast<float>(NonLocalMemoryInfo.Budget) / static_cast<float>(GB));
			Console::Write(L"\t\tCurrentUsage: %.2f GB\n", static_cast<float>(NonLocalMemoryInfo.CurrentUsage) / static_cast<float>(GB));
			Console::Write(L"\t\tAvailableForReservation: %.2f GB\n", static_cast<float>(NonLocalMemoryInfo.AvailableForReservation) / static_cast<float>(GB));
			Console::Write(L"\t\tCurrentReservation: %.2f GB\n", static_cast<float>(NonLocalMemoryInfo.CurrentReservation) / static_cast<float>(GB));
		}
		else
		{
			Status = FALSE;
			Console::Write(L"Error: Could not get adapter non-local memory info\n");
		}
	}

	if (Status == TRUE)
	{
		DXGI_QUERY_VIDEO_MEMORY_INFO LocalMemoryInfo = {};

		if (m_pIDxgiAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &LocalMemoryInfo) == S_OK)
		{
			Console::Write(L"\tLocal Memory Info:\n");
			Console::Write(L"\t\tBudget: %.2f GB\n", static_cast<float>(LocalMemoryInfo.Budget) / static_cast<float>(GB));
			Console::Write(L"\t\tCurrentUsage: %.2f GB\n", static_cast<float>(LocalMemoryInfo.CurrentUsage) / static_cast<float>(GB));
			Console::Write(L"\t\tAvailableForReservation: %.2f GB\n", static_cast<float>(LocalMemoryInfo.AvailableForReservation) / static_cast<float>(GB));
			Console::Write(L"\t\tCurrentReservation: %.2f GB\n", static_cast<float>(LocalMemoryInfo.CurrentReservation) / static_cast<float>(GB));
		}
		else
		{
			Status = FALSE;
			Console::Write(L"Error: Could not get adapter local memory info\n");
		}
	}

	return Status;
}

IRenderer* CGfxDevice::CreateRenderer(const RENDERER_DESC& rDesc)
{
	BOOL Status = TRUE;

	IRenderer* pIRenderer = NULL;
	ID3D12PipelineState* pIPipelineState = NULL;
	D3D12_INPUT_ELEMENT_DESC InputElementDescs[MAX_INPUT_ELEMENTS] = {};
	
	if (rDesc.InputLayout.NumInputs <= MAX_INPUT_ELEMENTS)
	{
		for (UINT i = 0; (Status == TRUE) && (i < rDesc.InputLayout.NumInputs); i++)
		{
			switch (rDesc.InputLayout.pInputElements[i].Element)
			{
				case INPUT_ELEMENT_POSITION:
				{
					InputElementDescs[i].SemanticName = "POSITION";
					break;
				}
				case INPUT_ELEMENT_COLOR:
				{
					InputElementDescs[i].SemanticName = "COLOR";
					break;
				}
				default:
				{
					Status = FALSE;
					Console::Write(L"Error: Invalid semantic index %u\n", rDesc.InputLayout.pInputElements[i].Element);
					break;
				}
			}

			if (Status == TRUE)
			{
				switch (rDesc.InputLayout.pInputElements[i].ElementFormat)
				{
					case INPUT_ELEMENT_FORMAT_RGB_32F:
					{
						InputElementDescs[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
						break;
					}
					case INPUT_ELEMENT_FORMAT_RGBA_32F:
					{
						InputElementDescs[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
						break;
					}
					default:
					{
						Status = FALSE;
						Console::Write(L"Error: Invalid element format: %u\n", rDesc.InputLayout.pInputElements[i].ElementFormat);
						break;
					}
				}
			}

			if (Status == TRUE)
			{
				switch (rDesc.InputLayout.pInputElements[i].Type)
				{
					case INPUT_ELEMENT_TYPE_PER_VERTEX:
					{
						InputElementDescs[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
						break;
					}
					case INPUT_ELEMENT_TYPE_PER_INSTANCE:
					{
						InputElementDescs[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
						break;
					}
					default:
					{
						Status = FALSE;
						Console::Write(L"Error: Invalid element type %u\n", rDesc.InputLayout.pInputElements[i].Type);
						break;
					}
				}
			}

			if (Status == TRUE)
			{
				InputElementDescs[i].AlignedByteOffset    = rDesc.InputLayout.pInputElements[i].AlignedByteOffset;
				InputElementDescs[i].InputSlot            = rDesc.InputLayout.pInputElements[i].InputSlot;
				InputElementDescs[i].SemanticIndex        = rDesc.InputLayout.pInputElements[i].ElementIndex;
				InputElementDescs[i].InstanceDataStepRate = rDesc.InputLayout.pInputElements[i].InstanceStep;
			}
		}
	}
	else
	{
		Status = FALSE;
		Console::Write(L"Error: Too many input elements\n");
	}

	if (Status == TRUE)
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC Desc;
		
		ZeroMemory(&Desc, sizeof(Desc));

		// Root Signature
		Desc.pRootSignature     = m_pID3D12RootSignature;

		// Vertex Shader
		Desc.VS.pShaderBytecode = rDesc.VertexShader.pCode;
		Desc.VS.BytecodeLength  = rDesc.VertexShader.Size;

		// Pixel Shader
		Desc.PS.pShaderBytecode = rDesc.PixelShader.pCode;
		Desc.PS.BytecodeLength  = rDesc.PixelShader.Size;

		// Domain Shader
		Desc.DS.pShaderBytecode = NULL;
		Desc.DS.BytecodeLength  = 0;

		// Hull Shader
		Desc.HS.pShaderBytecode = NULL;
		Desc.HS.BytecodeLength  = 0;

		// Geometry Shader
		Desc.GS.pShaderBytecode = NULL;
		Desc.GS.BytecodeLength  = 0;
	
		// Stream Output
		Desc.StreamOutput.pSODeclaration   = NULL;
		Desc.StreamOutput.NumEntries       = 0;
		Desc.StreamOutput.pBufferStrides   = NULL;
		Desc.StreamOutput.NumStrides       = 0;
		Desc.StreamOutput.RasterizedStream = 0;

		// Blend State
		Desc.BlendState.AlphaToCoverageEnable                     = FALSE;
		Desc.BlendState.IndependentBlendEnable                    = FALSE;
		for (unsigned int i = 0; i < 8; i++)
		{
			Desc.BlendState.RenderTarget[i].BlendEnable           = FALSE;
			Desc.BlendState.RenderTarget[i].LogicOpEnable         = FALSE;
			Desc.BlendState.RenderTarget[i].SrcBlend              = D3D12_BLEND_ONE;
			Desc.BlendState.RenderTarget[i].DestBlend             = D3D12_BLEND_ZERO;
			Desc.BlendState.RenderTarget[i].BlendOp               = D3D12_BLEND_OP_ADD;
			Desc.BlendState.RenderTarget[i].SrcBlendAlpha         = D3D12_BLEND_ONE;
			Desc.BlendState.RenderTarget[i].DestBlendAlpha        = D3D12_BLEND_ZERO;
			Desc.BlendState.RenderTarget[i].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
			Desc.BlendState.RenderTarget[i].LogicOp               = D3D12_LOGIC_OP_NOOP;
			Desc.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		}

		// Sample Mask
		Desc.SampleMask = UINT_MAX;

		// Rasterizer State
		Desc.RasterizerState.FillMode              = D3D12_FILL_MODE_SOLID;
		Desc.RasterizerState.CullMode              = D3D12_CULL_MODE_BACK;
		Desc.RasterizerState.FrontCounterClockwise = FALSE;
		Desc.RasterizerState.DepthBias             = D3D12_DEFAULT_DEPTH_BIAS;
		Desc.RasterizerState.DepthBiasClamp        = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		Desc.RasterizerState.SlopeScaledDepthBias  = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		Desc.RasterizerState.DepthClipEnable       = TRUE;
		Desc.RasterizerState.MultisampleEnable     = FALSE;
		Desc.RasterizerState.AntialiasedLineEnable = FALSE;
		Desc.RasterizerState.ForcedSampleCount     = 0;
		Desc.RasterizerState.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		// Depth Stencil State
		Desc.DepthStencilState.DepthEnable                  = FALSE;
		Desc.DepthStencilState.DepthWriteMask               = D3D12_DEPTH_WRITE_MASK_ZERO;
		Desc.DepthStencilState.DepthFunc                    = static_cast<D3D12_COMPARISON_FUNC>(0);
		Desc.DepthStencilState.StencilEnable                = FALSE;
		Desc.DepthStencilState.StencilReadMask              = 0;
		Desc.DepthStencilState.StencilWriteMask             = 0;
		Desc.DepthStencilState.FrontFace.StencilFailOp      = static_cast<D3D12_STENCIL_OP>(0);
		Desc.DepthStencilState.FrontFace.StencilDepthFailOp = static_cast<D3D12_STENCIL_OP>(0);
		Desc.DepthStencilState.FrontFace.StencilPassOp      = static_cast<D3D12_STENCIL_OP>(0);
		Desc.DepthStencilState.FrontFace.StencilFunc        = static_cast<D3D12_COMPARISON_FUNC>(0);
		Desc.DepthStencilState.BackFace.StencilFailOp       = static_cast<D3D12_STENCIL_OP>(0);
		Desc.DepthStencilState.BackFace.StencilDepthFailOp  = static_cast<D3D12_STENCIL_OP>(0);
		Desc.DepthStencilState.BackFace.StencilPassOp       = static_cast<D3D12_STENCIL_OP>(0);
		Desc.DepthStencilState.BackFace.StencilFunc         = static_cast<D3D12_COMPARISON_FUNC>(0);

		// Input Layout
		Desc.InputLayout.pInputElementDescs  = InputElementDescs;
		Desc.InputLayout.NumElements         = rDesc.InputLayout.NumInputs;

		// Index Buffer Strip Cut Value
		Desc.IBStripCutValue                 = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		// Primitive Topology Type
		Desc.PrimitiveTopologyType           = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// Num Render Targets
		Desc.NumRenderTargets                = 1;

		// Render Target Formats
		Desc.RTVFormats[0]                   = DXGI_FORMAT_R8G8B8A8_UNORM;;

		// Depth Stencil Format
		Desc.DSVFormat                       = DXGI_FORMAT_UNKNOWN;

		// Sample Desc
		Desc.SampleDesc.Count                = 1;
		Desc.SampleDesc.Quality              = DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;

		// Node Mask
		Desc.NodeMask                        = 0;

		// Cached Pipeline State Object
		Desc.CachedPSO.pCachedBlob           = NULL;
		Desc.CachedPSO.CachedBlobSizeInBytes = 0;
		
		Desc.Flags                           = D3D12_PIPELINE_STATE_FLAG_NONE;

		if (m_pID3D12Device->CreateGraphicsPipelineState(&Desc, __uuidof(ID3D12PipelineState), reinterpret_cast<VOID**>(&pIPipelineState)) != S_OK)
		{
			Status = false;
			Console::Write(L"Error: Failed to create graphics pipeline state object\n");
		}
	}

	if (Status == TRUE)
	{
		pIRenderer = CRenderer::CreateInstance(pIPipelineState);
	}

	return pIRenderer;
}

VOID CGfxDevice::DestroyRenderer(IRenderer* pIRenderer)
{
	if (pIRenderer != NULL)
	{
		CRenderer::DestroyInstance(static_cast<CRenderer*>(pIRenderer));
	}
}

ICommandList* CGfxDevice::CreateCommandList(VOID)
{
	ICommandList* pICommandList = NULL;
	ID3D12GraphicsCommandList* pInterface;

	if (m_pID3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pID3D12CommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), reinterpret_cast<VOID**>(&pInterface)) != S_OK)
	{
		pICommandList = CCommandList::CreateInstance(pInterface);
	}

	return pICommandList;
}

VOID CGfxDevice::DestroyCommandList(ICommandList* pICommandList)
{
	if (pICommandList != NULL)
	{
		CCommandList::DestroyInstance(static_cast<CCommandList*>(pICommandList));
		pICommandList = NULL;
	}
}

IFence* CGfxDevice::CreateFence(VOID)
{
	IFence* pIFence = NULL;
	ID3D12Fence* pInterface = NULL;

	if (m_pID3D12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), reinterpret_cast<VOID**>(&pInterface)) == S_OK)
	{
		pIFence = CFence::CreateInstance(pInterface);
	}

	return pIFence;
}

VOID CGfxDevice::DestroyFence(IFence* pIFence)
{
	if (pIFence != NULL)
	{
		CFence::DestroyInstance(static_cast<CFence*>(pIFence));
		pIFence = NULL;
	}
}
