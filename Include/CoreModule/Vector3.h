#pragma once

#ifndef M_CORE_VECTOR3
#define M_CORE_VECTOR3

#include <iostream>
#include <Vector2.h>

namespace MGameEngine
{
  inline namespace CoreModule
  {
    struct Vector3 final
        {
          // std::cout overload
          friend std::ostream& operator<<(std::ostream& o, const Vector3& v3);

          // Member
          float x;
          float y;
          float z;

        // Static Properties
        public:
          /// <summary>
          /// Vector3(0,0,0) (x = 0, y = 0, z = 0)
          /// </summary>
          static const Vector3 Zero;
          /// <summary>
          /// Vector3(-1,0,0) (x = -1, y = 0, z = 0)
          /// </summary>
          static const Vector3 Left;
          /// <summary>
          /// Vector3(1,0,0) (x = 1, y = 0, z = 0)
          /// </summary>
          static const Vector3 Right;
          /// <summary>
          /// Vector3(0,1,0) (x = 0, y = 1, z = 0)
          /// </summary>
          static const Vector3 Up;
          /// <summary>
          /// Vector3(0,-1,0) (x = 0, y = -1, z = 0)
          /// </summary>
          static const Vector3 Down;
          /// <summary>
          /// Vector3(0,0,1) (x = 0, y = 0, z = 1)
          /// </summary>
          static const Vector3 Forward;
          /// <summary>
          /// Vector3(0,0,-1) (x = 0, y = 0, z = -1)
          /// </summary>
          static const Vector3 Back;
          /// <summary>
          /// Vector3(1,1,1) (x = 1, y = 1, z = 1)
          /// </summary>
          static const Vector3 One;
          /// <summary>
          /// Vector3(INFINITY,INFINITY) (x = inf, y = inf, z = inf)
          /// </summary>
          static const Vector3 PositiveInfinity;
          /// <summary>
          /// Vector3(-INFINITY,-INFINITY) (x = -inf, y = -inf, z = -inf)
          /// </summary>
          static const Vector3 NegativeInfinity;

        // ---------------------------------------------------------
        // Constructor and Operator overload
        //		: x(value)			
        // 		, y(value)			
        // 		, z(value)
        // ---------------------------------------------------------
        public:
          constexpr Vector3()															
            : x(0.f)		
            , y(0.f)		
            , z(0.f)
          {}

          explicit Vector3(const float _x)									
            : x(_x)				
            , y(0.f)		
            , z(0.f)
          {}

          Vector3(const float _x, 
                    const float _y)					
            : x(_x)				
            , y(_y)				
            , z(0.f)
          {}

          Vector3(const float _x, 
                    const float _y,
                    const float _z)
            : x(_x)				
            , y(_y)				
            , z(_z)					
          {}
          Vector3(const Vector2 _v2, const float _z)					
            : x(_v2.x)			, y(_v2.y)			, z(_z)				
          {}

          // Copy
          Vector3(const Vector2& other)										
            : x(other.x)		
            , y(other.y)		
            , z(0.f)
          {}
          Vector3(const Vector3& other)										
            : x(other.x)		
            , y(other.y)		
            , z(other.z)	
          {}

          // Move
          Vector3(Vector3&& other) noexcept									
            : x(other.x)		
            , y(other.y)		
            , z(other.z)	
          { 
            other.x = 0.f;
            other.y = 0.f;
            other.z = 0.f;
          }

          // Operator overload

          // Arithmetic(need to be global)
          friend Vector3	operator+(const Vector3& self, const Vector2& other);
          friend Vector3	operator+(const Vector3& self, const Vector3& other);
          friend Vector3	operator-(const Vector3& self, const Vector2& other);
          friend Vector3	operator-(const Vector3& self, const Vector3& other);
          friend Vector3	operator*(const Vector3& self, const float num);
          friend Vector3	operator*(const float num, const Vector3& self);
          friend Vector3	operator*(const Vector3& self, const Vector3& other);
          friend Vector3	operator/(const Vector3& self, const float num);

          // Equality Compare
          friend bool		operator==(const Vector3& self, const Vector3& other);
          friend bool		operator!=(const Vector3& self, const Vector3& other);

          // Assignment
          Vector3& operator=(const Vector2& other) &;
          Vector3& operator=(const Vector3& other) &;
          Vector3& operator=(Vector3&& other) & noexcept;

          // Unary Negation/Plus
          Vector3 operator+() const;
          Vector3 operator-() const;

          // Cast
          explicit operator Vector2() const noexcept;
          explicit operator Vector3() const noexcept;

          // Compound Assignment
          Vector3&	operator +=	(const Vector2& other);
          Vector3&	operator +=	(const Vector3& other);
          Vector3&	operator -=	(const Vector2& other);
          Vector3&	operator -=	(const Vector3& other);
          Vector3&	operator *=	(const float num);
          Vector3&	operator *=	(const Vector3& other);
          Vector3&	operator /=	(const float num);
          
        // Public method
        public:
          auto GetMagnitude()  const	-> float;
          auto GetNormalized() const	-> Vector3;
          void Normalize();

        };
  }
}


#endif