#pragma once

#ifndef M_POOL
#define M_POOL

#include <iostream>
#include <functional>	// std::function
#include <mutex>		// std::mutex
#include "Memory-Release-Def.h"
#include "Thread-Safe-Def.h"

namespace MUtil
{
	namespace
	{
		constexpr uint32_t MAX_POOL_SIZE = 1000;
		constexpr uint32_t DEFAULT_POOL_SIZE = 20;

		inline void SetValidSize(int& size)
		{

			if(size <= 0)
			{
				#ifdef _DEBUG
					std::cout << "Pool receive invalid size.\nSet to default value = " << DEFAULT_POOL_SIZE << '\n';
				#endif
				size = (int)DEFAULT_POOL_SIZE;
			}

			else if(size > MAX_POOL_SIZE)
			{
				#ifdef _DEBUG
					std::cout << "Pool receive too big size.\nSet to max value = " << MAX_POOL_SIZE << '\n';
				#endif

				size = (int)MAX_POOL_SIZE;
			}	
		}
	}// nameless namespace

	template <typename obj>
	class IPool
	{
	public:
		virtual obj* Allocate() = 0;
		virtual void Recycle(obj* recycleObj) = 0;
		virtual void InitPool(std::function<obj*(void* const targetAddress)> factory) = 0;

	// Getter
	public:
		virtual bool IsEmpty() const = 0;
		virtual bool IsFull() const = 0;
		virtual int GetCapacity() const = 0;
		virtual int GetCount() const = 0;

	public:
		virtual ~IPool() {}
	};
	// End IPool

	/// @brief Pool
	/// @tparam T type
	template<typename T>
	class Pool final: public IPool<T>
	{
	// copy disable
	private:
		Pool(const Pool& rhs) = delete;
		Pool& operator = (const Pool& rhs) = delete;

	// interface implements
	public:
		T* Allocate() override;
		void Recycle(T* recycleObj) override;
		void InitPool(std::function<T* (void* const targetAddress)> factory) override;

	// IPool

	public:
		inline bool IsEmpty() const override { return _headIndex == _tailIndex && (_rwSign == 0b0);}
		inline bool IsFull() const override {	return _headIndex == _tailIndex && (_rwSign == 0b1);}
		inline int GetCapacity() const override { return _poolSize;}
		inline int GetCount() const override
		{
			return (_poolSize - _headIndex + _tailIndex) % _poolSize + (IsFull() ? _poolSize : 0);
		}

	public:
		explicit Pool(int size = 0) 
			: _pPool(nullptr)
			, _poolSize(0)
			, _headIndex(0)
			, _tailIndex(0)
			, _rwSign(0b0)
		{
			SetValidSize(size);
			_poolSize = static_cast<uint32_t>(size);
		}

		virtual ~Pool()
		{
			LOCK(_mutex);

			for(int i = 0;i < _poolSize; ++i)
			{
				_pPool[i].~T();
			}

			SAVE_FREE(_pPool);
			SAVE_FREE(_pAddressBuffer);

		}

	// Method
	private:

	protected:
		T *_pPool;
		T **_pAddressBuffer;
		uint16_t _poolSize;
		uint16_t _headIndex;
		uint16_t _tailIndex;

	private:
		std::mutex _mutex;		 
		uint8_t _rwSign : 1;	
	 
	};// class Pool  

	/// @brief allocate obj 
	/// @tparam T type
	/// @return ref of allocated obj
	template <typename T>
	T* Pool<T>::Allocate()
	{
		LOCK(_mutex);

		if(IsEmpty())
		{
			#ifdef  _DEBUG
				std::cout << "Can't allocate object from pool because pool is EMPTY!!\n" ;
			#endif //  DEBUG
			return nullptr;
		}

		if (_pAddressBuffer != nullptr)
		{
			T* alloc = _pAddressBuffer[_headIndex];
			_headIndex = (_headIndex + 1) % _poolSize;
			_rwSign = 0;

			return alloc;
		}

		return nullptr;
	}

	/// @brief recycle obj 
	/// @tparam T type
	/// @param recycleObj 
	template <typename T>
	void Pool<T>::Recycle(T* recycleObj)
	{
		LOCK(_mutex);

		if(IsFull())
		{
			#ifdef  _DEBUG
				std::cout << "Can't recycle object into pool because pool is FULL!!\n";
			#endif //  DEBUG
			return;
		}
		if (_pAddressBuffer != nullptr)
		{
			_pAddressBuffer[_tailIndex] = recycleObj;
			_tailIndex = (_tailIndex + 1) % _poolSize;	
			_rwSign = 1;
		}
	}

	/// @brief 
	/// @tparam 
	template<typename T>
	void Pool<T>::InitPool(std::function<T* (void* const targetAddress)> factory)
	{
		_pPool = static_cast<T*>(malloc(sizeof(T) * _poolSize));

		_pAddressBuffer = static_cast<T**>(malloc(sizeof(void*) * _poolSize));

		if (_pPool != nullptr && _pAddressBuffer != nullptr)
		{
			for (int i = 0; i < _poolSize; ++i)
			{
				_pAddressBuffer[i] = factory(_pPool + i);
			}

			_rwSign = 1;
		}
		else
		{
			SAVE_FREE(_pPool);
			SAVE_FREE(_pAddressBuffer);

			#ifdef _DEBUG
				std::cout << "Can't init pool because memory is not enough!!\n" << std::endl;
			#endif
		}
	}
}// namespace MUtil

#endif