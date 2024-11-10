/*

MRenderFramework
Author : MAI ZHICONG

Description : Shader Blob Wrapper (Graphics API: DirectX12)

Update History: 2024/11/10 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#pragma once

#ifndef M_DX12_SHADER_RES
#define M_DX12_SHADER_RES

#include <ComPtr.h>
#include <Class-Def-Macro.h>
#include <Interfaces/IDisposable.h>

struct ID3D10Blob;

namespace MFramework
{
  inline namespace MGraphics_DX12
  {
    class ShaderResBlob final : public IDisposable
    {
      GENERATE_CLASS_NO_COPY(ShaderResBlob)

      public:
        bool InitFromCSO(const wchar_t* fileName);
        bool InitFromFile(const wchar_t* fileName, 
                          const char* entryPoint,
                          const char* shaderModel 
                         );
        ID3D10Blob* Get(void) const;

      public:
        void Dispose(void) noexcept override;

      private:
        ComPtr<ID3D10Blob> m_shaderBlob;
    };

    inline ID3D10Blob* ShaderResBlob::Get() const
    {
      return m_shaderBlob.Get();
    }
  }
}

#endif