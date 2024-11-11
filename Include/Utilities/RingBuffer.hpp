// ---------------------------------------------------------------------------------------------------------------------------------------
// File name:           ring_buffer.h
// Version:             v1.0
// Description:         A template ring buffer class
// Namespace:           MUtil
// Note:                
//                          
// Update:              2024/06/17  Create
//
// Author:              MAI ZHICONG
// ---------------------------------------------------------------------------------------------------------------------------------------

#pragma once

#ifndef M_RING_BUFFER
#define M_RING_BUFFER

#include <cstdint>
#include <iostream>
#include <mutex>
#include "Memory-Release-Def.h"

namespace MUtil
{
	namespace
	{
		constexpr uint32_t RING_BUFFER_MAX_CAPACITY = 1000;
		constexpr uint32_t RING_BUFFER_DEFAULT_CAPACITY = 20;

		template<typename T>
		class IFIFO
		{
			public:
				virtual void Enqueue(const T& pushInstance) = 0;
				virtual void Dequeue(T& popInstance) = 0;
				virtual ~IFIFO() {}
		};
	}

	/// @brief RingBuffer
	template <typename T>
	class RingBuffer:public IFIFO<T>
	{
		public:
			RingBuffer();
			~RingBuffer();

		public:
			bool Init(int size);

		public:
			void Enqueue(const T& pushInstance) override final;
			void Dequeue(T& popInstance) override final;

		public:
			uint32_t GetCapacity() const { return m_capacity; }
			uint32_t GetCount() const { return m_count; }
			uint32_t GetUsedCount() const { return m_capacity - m_count; }

			T* const GetHeadAddress() const { return m_dataBuffer; }
			bool IsFull() const { return ((m_tailIndex + 1) % m_capacity) == m_headIndex; }
			bool IsEmpty() const { return (m_headIndex == m_tailIndex); }

		private:
			T* m_dataBuffer;
			uint32_t m_headIndex;
			uint32_t m_tailIndex;
			uint32_t m_capacity;
			uint32_t m_count;
			std::mutex m_mutex;

		private:
			RingBuffer(const RingBuffer& rhs) = delete;
			RingBuffer& operator= (const RingBuffer& rhs) = delete;
	};
	// End RingBuffer

#pragma region RingBuffer Definition

	template<typename T>
	RingBuffer<T>::RingBuffer()
		: m_dataBuffer(nullptr)
		, m_headIndex(0)
		, m_tailIndex(0)
		, m_capacity(0)
		, m_count(0)
	{
		//#ifdef _DEBUG
		//	std::cout << "Create ring buffer" << std::endl;
		//#endif
	}

	template<typename T>
	RingBuffer<T>::~RingBuffer()
	{
		for (int i = 0; i < m_capacity; ++i)
		{
			m_dataBuffer[i].~T();
		}
		// save release memory
		SAVE_FREE(m_dataBuffer)

	}

	/// @brief ring buffer initialize (call this method before use)
	/// @tparam T type store in ring buffer
	/// @param capacity 
	template<typename T>
	bool RingBuffer<T>::Init(int capacity)
	{
		// set default capacity when receive negative value
		if (capacity <= 0)
		{
			m_capacity = RING_BUFFER_DEFAULT_CAPACITY;
			//#ifdef _DEBUG
			//	std::cout << "receive negative value,use default size " << RING_BUFFER_DEFAULT_SIZE << std::endl;
			//#endif
		}
		// size limitation RING_BUFFER_MAX_SIZE
		else if(capacity > RING_BUFFER_MAX_CAPACITY)
		{
			m_capacity = RING_BUFFER_MAX_CAPACITY;
			//#ifdef _DEBUG
			//	std::cout << "receive exceeded value,use max size " << RING_BUFFER_MAX_SIZE << std::endl;
			//#endif
		}
		else
		{
			m_capacity = (uint32_t)capacity;
		}

		m_dataBuffer = static_cast<T*>(malloc(sizeof(T) * m_capacity));

		if(m_dataBuffer == nullptr)
		{
			return false;
		}

		++m_headIndex;
		m_count = m_capacity;

		return true;
	}

	/// @brief push obj in ring buffer
	/// @tparam T type store in ring buffer
	/// @param pushInstance push obj
	template<typename T>
	void RingBuffer<T>::Enqueue(const T& pushInstance)
	{
		// not initialized
		if(m_dataBuffer == nullptr)
			return;

		// buffer full
		if (IsFull())
		{
			return;
		}

		// push obj
		else
		{
			LOCK(m_mutex)

			_tailIndex = (_tailIndex + 1) % _capacity;
			m_dataBuffer[m_tailIndex] = std::move(pushInstance);

			++m_count;
		}
	}

	/// @brief pop obj
	/// @tparam T type store in ring buffer
	/// @param popInstance pop obj
	template<typename T>
	void RingBuffer<T>::Dequeue(T& popInstance)
	{
		// not initialized
		if(m_dataBuffer == nullptr)
			return;
			
		// buffer empty
		if (IsEmpty())
		{
			#ifdef _DEBUG
				std::cout << "Ring buffer is empty" << std::endl;
			#endif //  DEBUG
			return;
		}

		// pop obj
		else
		{
			LOCK(m_mutex)

			popInstance = std::move(_pData[_headIndex]);
			/*
			#ifdef  DEBUG
				std::cout << "Ring buffer pop method called" << std::endl;
			  	std::cout << "Pop Index :" << _headIndex << std::endl;
			#endif //  DEBUG
			*/
			m_headIndex = (m_headIndex + 1) % m_capacity;

			--m_count;
		}
	}

#pragma endregion // RingBuffer Definition

}// namespace MUtil

#endif