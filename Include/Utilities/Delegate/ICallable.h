/*

MRenderFramework
Author : MAI ZHICONG

Description : Delegate interface

Update History: 2024/12/17 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#ifndef M_ICALLABLE
#define M_ICALLABLE

#include <type_traits>
#include <cassert>

namespace MDelegate
{
  template<typename> 
  class ICallable;

  template<typename>
  class NormalFunctionDelegateInstance;

  template<typename ReturnType, typename... ArgTypes>
  class ICallable<ReturnType(ArgTypes...)>
  {
    public:
      virtual ~ICallable() { unbind();}
      virtual ReturnType Invoke(ArgTypes...) const = 0;
      virtual bool InvokeSafe(ArgTypes...) const = 0;
    protected:
      virtual void unbind() = 0;
  };

  template<typename ReturnType, typename... ArgTypes>
  class NormalFunctionDelegateInstance<ReturnType(ArgTypes...)> : public ICallable<ReturnType(ArgTypes...)>
  {
    using FuncPtr = ReturnType(*)(ArgTypes...);

    private:
      FuncPtr m_funcPtr;
  };
}



#endif