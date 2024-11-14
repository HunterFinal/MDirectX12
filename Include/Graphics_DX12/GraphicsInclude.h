/*

MRenderFramework
Author : MAI ZHICONG

Description : DirectX12 Graphics System Include List (Graphics API: DirectX12)

Update History: 2024/11/12 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#pragma once

#ifndef M_GRAPHICS_INCLUDE
#define M_GRAPHICS_INCLUDE

// D3D12 include
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

// D3D12 library include
#include <DirectXTex.h>

// wrapper class header include
#include <Graphics_DX12/DX12Device.h>
#include <Graphics_DX12/DX12DXGIFactory.h>
#include <Graphics_DX12/CommandList.h>
#include <Graphics_DX12/CommandQueue.h>
#include <Graphics_DX12/RootSignature.h>
#include <Graphics_DX12/PipelineState.h>
#include <Graphics_DX12/DX12SwapChain.h>
#include <Graphics_DX12/Fence.h>
#include <Graphics_DX12/VertexBufferContainer.h>
#include <Graphics_DX12/IndexBufferContainer.h>
#include <Graphics_DX12/DescriptorHandle.h>
#include <Graphics_DX12/DescriptorHeap.h>
#include <Graphics_DX12/ConstantBuffer.h>
#include <Graphics_DX12/RenderTarget.h>
#include <Graphics_DX12/ShaderResBlob.h>

#endif
