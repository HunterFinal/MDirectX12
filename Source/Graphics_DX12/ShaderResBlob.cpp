/*

MRenderFramework
Author : MAI ZHICONG

Description : Shader Blob Wrapper (Graphics API: DirectX12)

Update History: 2024/11/10 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#include <Graphics_DX12/ShaderResBlob.h>

#include <d3d12.h>

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include <FileUtil.h>

namespace
{
  #ifdef _DEBUG
    constexpr UINT SHADER_COMPILE_OPT = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;  // デバッグ用および最適化なし
  #else
    constexpr UINT SHADER_COMPILE_OPT = D3DCOMPILE_OPTIMIZATION_LEVEL3;  // 最高の最適化レベル
  #endif
}

namespace MFramework
{
  ShaderResBlob::ShaderResBlob()
    :m_shaderBlob(nullptr)
  { }

  ShaderResBlob::~ShaderResBlob()
  {
    Dispose();
  }

  bool ShaderResBlob::InitFromCSO(const wchar_t* fileName)
  {
    if (fileName == nullptr)
    {
      return false;
    }

    std::wstring destPath;
    if (!Utility::FileUtility::SearchFilePath(fileName, destPath))
    {
      return false;
    }

    HRESULT result = D3DReadFileToBlob(destPath.c_str(), m_shaderBlob.ReleaseAndGetAddressOf());

    if (FAILED(result))
    {
      return false;
    }

    return true;
  }

  bool ShaderResBlob::InitFromFile( const wchar_t* fileName, 
                                    const char* entryPoint,
                                    const char* shaderModel 
                                  )
  {
    if (fileName == nullptr || entryPoint == nullptr || shaderModel == nullptr)
    {
      return false;
    }

    HRESULT result = S_OK;
    ComPtr<ID3DBlob> error = nullptr;
    // 頂点シェーダー作成
    result = D3DCompileFromFile(
                                  fileName,                             // シェーダーファイル名
                                  nullptr,                              // #defineを配列で指定するための引数
                                  D3D_COMPILE_STANDARD_FILE_INCLUDE,    // shaderの中#includeが書かれているとき、インクルードファイルのディレクトリとしてカレントディレクトリを参照（#includeを使わないときはnullptrでよい）
                                  entryPoint,                           // エントリーポイント
                                  shaderModel,                          // シェーダーモデルバージョン
                                  SHADER_COMPILE_OPT,                   // シェーダーコンパイルオプション
                                  0,                                    // エフェクトコンパイルオプション（シェーダーファイルの場合０が推奨）
                                  m_shaderBlob.ReleaseAndGetAddressOf(),// 受け取るためのポインターのアドレス
                                  error.ReleaseAndGetAddressOf()        // エラー用ポインターのアドレス
    );

    if (FAILED(result))
    {
      if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
      {
        ::OutputDebugStringW(L"*****ERROR***** ファイルが見当たりません。\n");
      }
      else
      {
        if (error.Get() != nullptr)
        {
          ::OutputDebugStringA(static_cast<char*>(error->GetBufferPointer()));
        }
      }

      return false;
    }

    return true;
  }

  void ShaderResBlob::Dispose() noexcept
  {
    m_shaderBlob.Reset();
  }
}