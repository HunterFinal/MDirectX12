/*									Vector2	cpp  									*/
// ------------------------------------------------------------------------------------
// File name			:Vector2.cpp
// Summary				:Vector2 implement
// 
// Writer				:MAI ZHICONG
// Update Message		:2024/04/25		Create
//						:2024/04/27		Rewrite rules of operator overload
// ------------------------------------------------------------------------------------

#include "Vector2.h"

#include <cmath>

namespace
{
	constexpr float FLOAT_TOLERANCE = 0.000001f;
}

namespace MGameEngine
{
	inline namespace CoreModule
	{

		// Vector2 
		#pragma region Vector2
		// Static properties initialize 
		#pragma region Static properties initialize

			const Vector2 Vector2::Zero(0, 0);
			const Vector2 Vector2::Left(-1, 0);
			const Vector2 Vector2::Right(1, 0);
			const Vector2 Vector2::Up(0, 1);
			const Vector2 Vector2::Down(0, -1);
			const Vector2 Vector2::One(1, 1);
			const Vector2 Vector2::PositiveInfinity(INFINITY, INFINITY);
			const Vector2 Vector2::NegativeInfinity(-INFINITY, -INFINITY);

		#pragma endregion

		// Operator overload
		#pragma region Operator overload

		std::ostream& operator<<(std::ostream& o, const Vector2& v2)
		{
			o << '(' << v2.x << ',' << v2.y << ')';
			return o;
		}

		Vector2& Vector2::operator=(const Vector2& other) &
		{
			if (this != &other)
			{
				x = other.x;
				y = other.y;
			}
			return *this;
		}

		Vector2& Vector2::operator=(const Vector3& other) &
		{
			x = other.x;
			y = other.y;

			return *this;
		}

		Vector2& Vector2::operator=(Vector2&& other) & noexcept
		{
			if (this != &other)
			{
				x = other.x;
				y = other.y;

				other.x = (float)0;
				other.y = (float)0;
			}
			return *this;
		}

		Vector2 operator+(const Vector2& self, const Vector2& other)
		{
			Vector2 addVec;
			addVec.x = self.x + other.x;
			addVec.y = self.y + other.y;

			return addVec;
		}

		Vector2 operator+(const Vector2& self, const Vector3& other)
		{
			Vector2 addVec;
			addVec.x = self.x + other.x;
			addVec.y = self.y + other.y;

			return addVec;
		}

		Vector2& Vector2::operator+=(const Vector2& other) 
		{
			x += other.x;
			y += other.y;

			return *this;
		}

		Vector2& Vector2::operator+=(const Vector3& other)
		{
			x += other.x;
			y += other.y;

			return *this;
		}

		Vector2 operator-(const Vector2& self, const Vector2& other)
		{
			Vector2 subVec;
			subVec.x = self.x - other.x;
			subVec.y = self.y - other.y;

			return subVec;
		}

		Vector2 operator-(const Vector2& self, const Vector3& other)
		{
			Vector2 subVec;
			subVec.x = self.x - other.x;
			subVec.y = self.y - other.y;

			return subVec;
		}


		Vector2& Vector2::operator-=(const Vector2& other)
		{
			x -= other.x;
			y -= other.y;

			return *this;
		}

		Vector2& Vector2::operator-=(const Vector3& other)
		{
			x -= other.x;
			y -= other.y;

			return *this;
		}

		Vector2 operator *(const Vector2& self, const float num)
		{
			Vector2 mulVec;
			mulVec.x = self.x * num;
			mulVec.y = self.y * num;

			return mulVec;
		}


		Vector2 operator *(const float num, const Vector2& self)
		{
			return self * num;
		}

		Vector2 operator*(const Vector2& self, const Vector2& other)
		{
			Vector2 mulVec;
			mulVec.x = self.x * other.x;
			mulVec.y = self.y * other.y;

			return mulVec;
		}

		Vector2 operator /(const Vector2& self, const float num)
		{
			if (num <= FLOAT_TOLERANCE && num >= -FLOAT_TOLERANCE)
			{
				return Vector2::PositiveInfinity;
			}

			Vector2 divVec;
			divVec.x = self.x / num;
			divVec.y = self.y / num;
			return divVec;
		}

		Vector2& Vector2::operator*=(const float num)
		{
			x *= num;
			y *= num;

			return *this;
		}

		Vector2& Vector2::operator*=(const Vector2& other)
		{
			x *= other.x;
			y *= other.y;

			return *this;
		}



		Vector2& Vector2::operator/=(const float num)
		{
			if (num <= FLOAT_TOLERANCE && num >= -FLOAT_TOLERANCE)
			{
				x = PositiveInfinity.x;
				y = PositiveInfinity.y;
			}
			else
			{
				x /= num;
				y /= num;
			}
				return *this;
		}

		bool operator==(const Vector2& self, const Vector2& other)
		{
			// Check x
			if ((self.x - other.x) > -FLOAT_TOLERANCE && (self.x - other.x) < FLOAT_TOLERANCE)
			{
				// Check y
				if ((self.y - other.y) > -FLOAT_TOLERANCE && (self.y - other.y) < FLOAT_TOLERANCE)
					return true;
			}
			return false;

		}

		bool operator!=(const Vector2& self, const Vector2 & other)
		{
			return !(self == other);
		}

		Vector2 Vector2::operator-() const
		{
			Vector2 negative(*this);

			negative *= (float)-1;

			return negative;
		}

		Vector2 Vector2::operator+() const
		{
			Vector2 positive(*this);

			return positive;
		}
		
		Vector2::operator Vector2() const noexcept
		{
			return *this;
		}

		Vector2::operator Vector3() const noexcept
		{
			Vector3 cast (*this);

			return cast;
		}

	#pragma endregion

	// Public method
	#pragma region Public method
		float Vector2::GetMagnitude() const
		{
			return sqrtf((powf(x, (float)2) + powf(y, (float)2)));
		}

		Vector2 Vector2::GetNormalized() const
		{
			Vector2 normalize(*this);

			float tempManitude = GetMagnitude();

			if (tempManitude < FLOAT_TOLERANCE)
			{
				return Zero;
			}

			return normalize / tempManitude;
		}

		void Vector2::Normalize()
		{
			float tempMagnitude = GetMagnitude();

			if (tempMagnitude < FLOAT_TOLERANCE)
			{
				return;
			}

			x /= tempMagnitude;
			y /= tempMagnitude;

		}
	}

	#pragma endregion

#pragma endregion


#pragma endregion

}
// namespace MGameEngine


