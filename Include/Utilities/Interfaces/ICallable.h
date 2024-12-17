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
  class ICallable
  {
    public:
      virtual ~ICallable() { unbind();}
      virtual void operator()() const = 0;
    protected:
      virtual void unbind() = 0;
  };
}



#endif