#include "EnumTranslator.hpp"

#include <d3d12.h>

#include <Wdk.hpp>

BOOL EnumTranslator::CommandBufferType_To_CommandListType(COMMAND_BUFFER_TYPE CmdBufType, D3D12_COMMAND_LIST_TYPE& rCmdListType)
{
	BOOL Status = TRUE;

	switch (CmdBufType)
	{
		case COMMAND_BUFFER_TYPE_DIRECT:
		{
			rCmdListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
			break;
		}
		case COMMAND_BUFFER_TYPE_COMPUTE:
		{
			rCmdListType = D3D12_COMMAND_LIST_TYPE_COMPUTE;
			break;
		}
		case COMMAND_BUFFER_TYPE_COPY:
		{
			rCmdListType = D3D12_COMMAND_LIST_TYPE_COPY;
			break;
		}
		default:
		{
			Status = FALSE;
			Console::Write(L"Error: Invalid command buffer type %u\n", CmdBufType);
			rCmdListType = static_cast<D3D12_COMMAND_LIST_TYPE>(-1);
			break;
		}
	}

	return Status;
}

BOOL EnumTranslator::CommandQueueType_To_CommandListType(COMMAND_QUEUE_TYPE CmdQueueType, D3D12_COMMAND_LIST_TYPE& rCmdListType)
{
	BOOL Status = TRUE;

	switch (CmdQueueType)
	{
	case COMMAND_QUEUE_TYPE_DIRECT:
	{
		rCmdListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
		break;
	}
	case COMMAND_QUEUE_TYPE_COMPUTE:
	{
		rCmdListType = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		break;
	}
	case COMMAND_QUEUE_TYPE_COPY:
	{
		rCmdListType = D3D12_COMMAND_LIST_TYPE_COPY;
		break;
	}
	default:
	{
		Status = FALSE;
		Console::Write(L"Error: Invalid command queue type %u\n", CmdQueueType);
		rCmdListType = static_cast<D3D12_COMMAND_LIST_TYPE>(-1);
		break;
	}
	}

	return Status;
}

BOOL EnumTranslator::InputElement_To_SemanticName(INPUT_ELEMENT Element, PCCHAR& rSemanticName)
{
	BOOL Status = TRUE;

	switch (Element)
	{
		case INPUT_ELEMENT_POSITION:
		{
			rSemanticName = "POSITION";
			break;
		}
		case INPUT_ELEMENT_COLOR:
		{
			rSemanticName = "COLOR";
			break;
		}
		default:
		{
			Status = FALSE;
			Console::Write(L"Error: Invalid semantic index %u\n", Element);
			rSemanticName = "INVALID";
			break;
		}
	}

	return Status;
}

BOOL EnumTranslator::InputElementFormat_To_DxgiFormat(INPUT_ELEMENT_FORMAT ElementFormat, DXGI_FORMAT& rDxgiFormat)
{
	BOOL Status = TRUE;

	switch (ElementFormat)
	{
		case INPUT_ELEMENT_FORMAT_RGB_32F:
		{
			rDxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			break;
		}
		case INPUT_ELEMENT_FORMAT_RGBA_32F:
		{
			rDxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		}
		default:
		{
			Status = FALSE;
			Console::Write(L"Error: Invalid element format: %u\n", ElementFormat);
			rDxgiFormat = DXGI_FORMAT_UNKNOWN;
			break;
		}
	}

	return Status;
}

BOOL EnumTranslator::InputElementType_To_InputSlotClass(INPUT_ELEMENT_TYPE ElementType, D3D12_INPUT_CLASSIFICATION& rInputSlotClass)
{
	BOOL Status = TRUE;

	switch (ElementType)
	{
		case INPUT_ELEMENT_TYPE_PER_VERTEX:
		{
			rInputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			break;
		}
		case INPUT_ELEMENT_TYPE_PER_INSTANCE:
		{
			rInputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
			break;
		}
		default:
		{
			Status = FALSE;
			Console::Write(L"Error: Invalid element type %u\n", ElementType);
			rInputSlotClass = static_cast<D3D12_INPUT_CLASSIFICATION>(-1);
			break;
		}
	}

	return Status;
}
