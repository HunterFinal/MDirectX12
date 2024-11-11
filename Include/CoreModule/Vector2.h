/*									Vector2 header									*/
// ------------------------------------------------------------------------------------
// File name			:Vector2.h
// Summary				:Vector2 struct

// Writer				:MAI ZHICONG(�o�N �`�\�E)
// Update Message		:2024/04/25		Create
//						:2024/04/27		Rewrite rules of operator overload
// ------------------------------------------------------------------------------------

#pragma once

#ifndef M_CORE_VECTOR2
#define M_CORE_VECTOR2

#include <iostream>
#include <Vector3.h>

namespace MGameEngine
{
	inline namespace CoreModule
	{
		struct Vector2 final
		{
			// std::cout overload
			friend std::ostream& operator<<(std::ostream& o, const Vector2& v2);

			// Member
			float x;
			float y;
			
		// Static Properties
		public:
			/// <summary>
			/// Vector2(0,0) (x = 0,y = 0)
			/// </summary>
			static const Vector2 Zero;
			/// <summary>
			/// Vector2(-1,0) (x = -1,y = 0)
			/// </summary>
			static const Vector2 Left;
			/// <summary>
			/// Vector2(1,0) (x = 1,y = 0)
			/// </summary>
			static const Vector2 Right;
			/// <summary>
			/// Vector2(0,1) (x = 0,y = 1)
			/// </summary>
			static const Vector2 Up;
			/// <summary>
			/// Vector2(0,-1) (x = 0,y = -1)
			/// </summary>
			static const Vector2 Down;
			/// <summary>
			/// Vector2(1,1) (x = 1,y = 1)
			/// </summary>
			static const Vector2 One;
			/// <summary>
			/// Vector2(INFINITY,INFINITY) (x = inf, y = inf)
			/// </summary>
			static const Vector2 PositiveInfinity;
			/// <summary>
			/// Vector2(-INFINITY,-INFINITY) (x = -inf, y = -inf)
			/// </summary>
			static const Vector2 NegativeInfinity;

		// Constructor
		//		: x(value)		
		//		, y(value)
		public:
			constexpr Vector2()											
				: x(0.f)	
				, y(0.f)
			{}

			explicit Vector2(const float _x)					
				: x(_x)			
				, y(0.f)	
			{}

			Vector2(const float _x, const float _y)	
				: x(_x)			
				, y(_y)			
			{}

			// Copy
			Vector2(const Vector2& other)						
				: x(other.x)	
				, y(other.y)	
			{}
			// Move
			Vector2(Vector2&& other) noexcept					
				: x(other.x)	
				, y(other.y)	
			{	
				other.x = 0.f;
				other.y = 0.f; 
			}

			// Operator overload

			// Arithmetic(need to be global)
			friend Vector2	operator +	(const Vector2& self, const Vector2& other);
			friend Vector2	operator +	(const Vector2& self, const Vector3& other);
			friend Vector2	operator -	(const Vector2& self, const Vector2& other);
			friend Vector2	operator -	(const Vector2& self, const Vector3& other);
			friend Vector2	operator *	(const Vector2& self, const float num);
			friend Vector2	operator *	(const float num, const Vector2& self);
			friend Vector2	operator *	(const Vector2& self, const Vector2& other);
			friend Vector2	operator /	(const Vector2& self, const float num);

			// Equality Compare(need to be global)
			friend bool	operator ==	(const Vector2& self, const Vector2& other);
			friend bool	operator !=	(const Vector2& self, const Vector2& other);

			// Assignment
			Vector2& operator =	(const Vector2& other)&;
			Vector2& operator =	(const Vector3& other)&;
			Vector2& operator =	(Vector2&& other) & noexcept;

			// Unary Negation/Plus
			Vector2 operator+() const;
			Vector2 operator-() const;

			// Cast
			explicit operator Vector2() const noexcept;
			explicit operator Vector3() const noexcept;

			// Compound Assignment
			Vector2&	operator +=	(const Vector2& other);
			Vector2&	operator +=	(const Vector3& other);
			Vector2&	operator -=	(const Vector2& other);
			Vector2&	operator -=	(const Vector3& other);
			Vector2&	operator *=	(const float num);
			Vector2&	operator *=	(const Vector2& other);
			Vector2&	operator /=	(const float num);


		// Public method
		public:
			auto GetMagnitude()  const	-> float;
			auto GetNormalized() const	-> Vector2;
			void Normalize();

		}; 

	} 
	// namespace CoreModule
}
// namespace MGameEngine

#endif