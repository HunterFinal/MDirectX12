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

  template<typename,typename>
  class ClassMemberFunctionDelegateInstance;

  template<bool IsConst, typename ClassType, typename FuncType>
  struct ClassMemPtrType;

  template<typename ClassType, typename ReturnType, typename... ArgTypes>
  struct ClassMemPtrType<false, ClassType, ReturnType(ArgTypes...)>
  {
    using Type = ReturnType (ClassType::*)(ArgTypes...);
  };

  template<typename ClassType, typename ReturnType, typename... ArgTypes>
  struct ClassMemPtrType<true, ClassType, ReturnType(ArgTypes...)>
  {
    using Type = ReturnType (ClassType::*)(ArgTypes...) const;
  };

  template<typename ReturnType, typename... ArgTypes>
  class ICallable<ReturnType(ArgTypes...)>
  {
  public:
    using SelfType = ICallable<ReturnType(ArgTypes...)>;
  public:
    virtual ~ICallable() { unbind(); }
    virtual ReturnType Invoke(ArgTypes&&...) const = 0;
    virtual ReturnType operator()(ArgTypes&&...) const = 0;
    virtual ReturnType InvokeSafe(ArgTypes&&...) const = 0;
    virtual bool Equals(const SelfType*) const = 0;
  protected:
    virtual void unbind() {};
  };

  template<typename ReturnType, typename... ArgTypes>
  class NormalFunctionDelegateInstance<ReturnType(ArgTypes...)> : public ICallable<ReturnType(ArgTypes...)>
  {
    using FuncPtr = ReturnType(*)(ArgTypes...);
    using SelfType = NormalFunctionDelegateInstance<ReturnType(ArgTypes...)>;
    using SuperType = ICallable<ReturnType(ArgTypes...)>;

  public:
    NormalFunctionDelegateInstance()
      : NormalFunctionDelegateInstance(nullptr)
    { }
    explicit NormalFunctionDelegateInstance(std::nullptr_t)
      : m_funcPtr(nullptr)
    { }
    explicit NormalFunctionDelegateInstance(FuncPtr&& ptr) noexcept
      : m_funcPtr(std::forward<FuncPtr>(ptr))
    { }

    virtual ~NormalFunctionDelegateInstance()
    { }
    NormalFunctionDelegateInstance(const SelfType& other)
      : NormalFunctionDelegateInstance(nullptr)
    {
      *this = other;
    }

    NormalFunctionDelegateInstance& operator=(const SelfType& other)
    {
      if (this != &other)
      {
        m_funcPtr = other.m_funcPtr;
      }

      return *this;
    }
    NormalFunctionDelegateInstance(SelfType&& other) noexcept
      : NormalFunctionDelegateInstance(nullptr)
    {
      *this = std::move(other);
    }

    NormalFunctionDelegateInstance& operator=(SelfType&& other) noexcept
    {
      if (this != &other)
      {
        m_funcPtr = other.m_funcPtr;
        other.unbind();
      }

      return *this;
    }

    ReturnType operator()(ArgTypes&&... args) const
    {
      return Invoke(std::forward<ArgTypes>(args)...);
    }

    ReturnType Invoke(ArgTypes&&... args) const override
    {
      if constexpr (std::is_void_v<ReturnType>)
      {
        return (void)(*m_funcPtr)(std::forward<ArgTypes>(args)...);
      }
      else
      {
        return (*m_funcPtr)(std::forward<ArgTypes>(args)...);
      }
    }

    ReturnType InvokeSafe(ArgTypes&&... args) const override
    {
      if (m_funcPtr != nullptr)
      {
        return Invoke(std::forward<ArgTypes>(args)...);
      }
    }

    bool Equals(const SuperType* other) const
    {
      const SelfType* castOther = dynamic_cast<const SelfType*>(other);

      if (castOther == nullptr)
      {
        return false;
      }

      return this->m_funcPtr == castOther->m_funcPtr;
    }

  protected:
    void unbind() override
    {
      m_funcPtr = nullptr;
    }

  private:
    FuncPtr m_funcPtr;
  };

  template<typename ClassMemPtrType, typename ReturnType, typename... ArgTypes>
  class ClassMemberFunctionDelegateInstance<ClassMemPtrType, ReturnType(ArgTypes...)> : public ICallable<ReturnType(ArgTypes...)>
  {
    public:
      using FuncPtr = ClassMemPtrType::Type;
      using SelfType = ClassMemberFunctionDelegateInstance<ClassMemPtrType, ReturnType(ArgTypes...)>;
      using SuperType = ICallable<ReturnType(ArgTypes...)>;
    
    public:
      ClassMemberFunctionDelegateInstance()
        : ClassMemberFunctionDelegateInstance(nullptr)
      { }
      explicit ClassMemberFunctionDelegateInstance(std::nullptr_t)
        : m_funcPtr(nullptr)
      { }
      explicit ClassMemberFunctionDelegateInstance(FuncPtr&& ptr)
        : m_funcPtr(std::forward<FuncPtr>(ptr))
      { }

      virtual ~ClassMemberFunctionDelegateInstance()
      { }

      ReturnType operator()(ArgTypes&&... args) const override
      {
        return Invoke(std::forward<ArgTypes>(args)...);
      }

      ReturnType Invoke(ArgTypes&&... args) const override
      {
        if constexpr (std::is_void_v<ReturnType>)
        {
          return (void)(*m_funcPtr)(std::forward<ArgTypes>(args)...);
        }
        else
        {
          return (*m_funcPtr)(std::forward<ArgTypes>(args)...);
        }
      }

      ReturnType InvokeSafe(ArgTypes&&... args) const override
      {
      
      }

    private:
      FuncPtr m_funcPtr;


  };

}

#endif