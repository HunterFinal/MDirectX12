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
    template<typename Value_Type>
    class MPoolItem
    {
        public:
      Value_Type* ptr;
    };
  }
}

namespace MFramework
{
  inline namespace Utility
  {
    namespace
    {
      constexpr size_t MAX_POOL_CAPACITY = 1000;
    }

    template<typename Value_Type>
    class Pool : public IDisposable
    {
      ALIAS(Value_Type*, Value_Type_Ptr);
      GENERATE_CLASS_NO_COPY(Pool)

      public:
        GENERATE_CONSTRUCTOR(Pool, const size_t size, const size_t itemSize);

      public:
        void Dispose(void) noexcept override;
      
      public:
        MPoolItem<Value_Type>* Allocate();
        void Recycle(const MPoolItem<Value_Type>*);
        size_t GetSize() const;
        size_t GetCapacity() const;

      private:
        size_t m_capacity;
        size_t m_size;
        char* m_allocBuffer;
        std::vector<MPoolItem<Value_Type>*> m_useableItemList;
        mutable std::mutex m_mutex;
    };

    template<typename Value_Type>
    Pool<Value_Type>::Pool()
      : m_capacity(0)
      , m_size(0)
      , m_allocBuffer(nullptr)
      , m_useableItemList({})
      , m_mutex({})
    { 
      assert(false);
    }

    template<typename Value_Type>
    Pool<Value_Type>::Pool(const size_t size, const size_t itemSize)
    {
      assert(size > 0);
      assert(itemSize > 0);

      const auto tempSize = size >= MAX_POOL_CAPACITY ? MAX_POOL_CAPACITY : size;

      m_allocBuffer = new(std::nothrow) char[tempSize * itemSize];
      m_useableItemList.reserve(tempSize);

      for (size_t i = 0; i < tempSize; ++i)
      {
        auto address = new (m_allocBuffer + i * itemSize) Value_Type();

        MPoolItem<Value_Type>* temp = new MPoolItem<Value_Type>();

        temp->ptr = address;

        m_useableItemList.emplace_back(temp);

      }
    }

    template<typename Value_Type>
    Pool<Value_Type>::~Pool()
    {
      Dispose();
    }

    template<typename Value_Type>
    inline size_t Pool<Value_Type>::GetSize() const
    {
      return m_size;
    }

    template<typename Value_Type>
    inline size_t Pool<Value_Type>::GetCapacity() const
    {
      return m_capacity;
    }

    template<typename Value_Type>
    MPoolItem<Value_Type>* Pool<Value_Type>::Allocate()
    {
      return nullptr;
    }

    template<typename Value_Type>
    void Pool<Value_Type>::Recycle(const MPoolItem<Value_Type>*)
    {

    }

    template<typename Value_Type>
    void Pool<Value_Type>::Dispose() noexcept
    {

    }
  }
}

#endif