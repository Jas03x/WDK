#ifndef WDK_ENUM_TRANSLATOR_HPP
#define WDK_ENUM_TRANSLATOR_HPP

#include "WdkDef.hpp"
#include "WdkGfx.hpp"

enum DXGI_FORMAT;
enum D3D12_COMMAND_LIST_TYPE;
enum D3D12_INPUT_CLASSIFICATION;

class EnumTranslator
{
public:
	static BOOL CommandBufferType_To_CommandListType(COMMAND_BUFFER_TYPE CmdBufType, D3D12_COMMAND_LIST_TYPE& rCmdListType);
	static BOOL CommandQueueType_To_CommandListType(COMMAND_QUEUE_TYPE CmdQueueType, D3D12_COMMAND_LIST_TYPE& rCmdListType);

	static BOOL InputElement_To_SemanticName(INPUT_ELEMENT Element, PCCHAR& rSemanticName);
	static BOOL InputElementFormat_To_DxgiFormat(INPUT_ELEMENT_FORMAT ElementFormat, DXGI_FORMAT& rDxgiFormat);
	static BOOL	InputElementType_To_InputSlotClass(INPUT_ELEMENT_TYPE ElementType, D3D12_INPUT_CLASSIFICATION& rInputSlotClass);
};

#endif // WDK_ENUM_TRANSLATOR_HPP