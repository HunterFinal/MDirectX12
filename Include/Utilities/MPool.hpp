/*

MFramework

Author : MAI ZHICONG

Description : Pool template class

Update History: 2024/11/21 Create

Version : alpha_1.0.0

Encoding : UTF-8 

*/
#pragma once

#ifndef M_POOL
#define M_POOL

#include <ClassBaseInc.h>
#include "Thread-Safe-Def.h"

#include <vector>
#include <functional>
#include <cassert>

namespace MFramework
{
  inline namespace Utility
  {
    class MPoolItem;
  }
}

namespace MFramework
{
  inline namespace Utility
  {
    namespace
    {
      constexpr size_t MAX_POOL_CAPACITY = 1000;
      ALIAS(std::function<MPoolItem*(void)>, GenerateFunc)
    }

    class Pool : public IDisposable
    {
      GENERATE_CLASS_NO_COPY(Pool)

      public:
        GENERATE_CONSTRUCTOR(Pool, const size_t size, const size_t itemSize, const GenerateFunc func);

      public:
        void Dispose(void) noexcept override;
      
      public:
        MPoolItem* Allocate();
        void Recycle(MPoolItem*);
        size_t GetSize() const;
        size_t GetCapacity() const;

      private:
        size_t m_capacity;
        size_t m_size;
        char* m_allocBuffer;
        std::vector<MPoolItem*> m_useableItemList;
        std::mutex m_mutex;
    };

    Pool::Pool()
      : m_capacity(0)
      , m_size(0)
      , m_allocBuffer(nullptr)
      , m_useableItemList({})
      , m_mutex({})
    { 
      assert(false);
    }

    Pool::Pool(const size_t size, const size_t itemSize, const GenerateFunc func)
    {
      assert(size > 0);
      assert(itemSize > 0);
      assert(func);

      auto tempSize = size >= MAX_POOL_CAPACITY ? MAX_POOL_CAPACITY : size;

      m_allocBuffer = new(std::nothrow) char[tempSize * itemSize];
    }
  }
}

#endif