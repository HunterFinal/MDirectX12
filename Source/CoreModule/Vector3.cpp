/*

MGameEngine CoreModule
Author : MAI ZHICONG

Description : Vector3

Update History		:2024/04/25		Create
                  :2024/04/27		Rewrite rules of operator overload
                  :2024/11/10   Seperate Vector2 and Vector3

Version : alpha_1.0.0

Encoding : UTF-8 

*/

#include <Vector3.h>

namespace
{
	constexpr float FLOAT_TOLERANCE = 0.000001f;
}

namespace MGameEngine
{ 
  inline namespace CoreModule
  {

    // Vector3
    #pragma region Vector3
    // Static properties initialize 
    #pragma region Static properties initialize 
      const Vector3 Vector3::Zero(0, 0, 0);
      const Vector3 Vector3::Left(-1, 0, 0);
      const Vector3 Vector3::Right(1, 0, 0);
      const Vector3 Vector3::Up(0, 1, 0);
      const Vector3 Vector3::Down(0, -1, 0);
      const Vector3 Vector3::Forward(0, 0, 1);
      const Vector3 Vector3::Back(0, 0, -1);
      const Vector3 Vector3::One(1, 1, 1);
      const Vector3 Vector3::PositiveInfinity(INFINITY, INFINITY, INFINITY);
      const Vector3 Vector3::NegativeInfinity(-INFINITY, -INFINITY, -INFINITY);
    #pragma endregion

    // Operator overload
    #pragma region Operator overload

      std::ostream& operator<<(std::ostream& o, const Vector3& v3)
      {
        o << '(' << v3.x << ',' << v3.y << ',' << v3.z << ')';
        return o;
      }

      Vector3& Vector3::operator=(const Vector2& other) &
      {
        x = other.x;
        y = other.y;
        z = (float)0;

        return *this;
      }

      Vector3& Vector3::operator=(const Vector3& other) &
      {
        if (this != &other)
        {
          x = other.x;
          y = other.y;
          z = other.z;
        }

        return *this;
      }

      Vector3& Vector3::operator=(Vector3&& other) & noexcept
      {
        if (this != &other)
        {
          x = other.x;
          y = other.y;
          z = other.z;

          other.x = (float)0;
          other.y = (float)0;
          other.z = (float)0;
        }

        return *this;
      }

      Vector3 operator+(const Vector3& self, const Vector2& other)
      {
        Vector3 addVec;
        addVec.x = self.x + other.x;
        addVec.y = self.y + other.y;
        addVec.z = self.z;

        return addVec;
      }

      Vector3 operator+(const Vector3& self, const Vector3& other)
      {
        Vector3 addVec;
        addVec.x = self.x + other.x;
        addVec.y = self.y + other.y;
        addVec.z = self.z + other.z;

        return addVec;
      }

      Vector3& Vector3::operator+=(const Vector2& other)
      {
        x += other.x;
        y += other.y;

        return *this;
      }

      Vector3& Vector3::operator+=(const Vector3& other)
      {
        x += other.x;
        y += other.y;
        z += other.z;

        return *this;
      }
      Vector3 operator-(const Vector3& self, const Vector2& other)
      {
        Vector3 subVec;
        subVec.x = self.x - other.x;
        subVec.y = self.y - other.y;
        subVec.z = self.z;

        return subVec;
      }

      Vector3 operator-(const Vector3& self, const Vector3& other)
      {
        Vector3 subVec;
        subVec.x = self.x - other.x;
        subVec.y = self.y - other.y;
        subVec.z = self.z - other.z;

        return subVec;
      }

      Vector3& Vector3::operator-=(const Vector2& other)
      {
        x -= other.x;
        y -= other.y;

        return *this;
      }

      Vector3& Vector3::operator-=(const Vector3& other)
      {
        x -= other.x;
        y -= other.y;
        z -= other.z;

        return *this;
      }

      Vector3 operator*(const Vector3& self, const float num)
      {
        Vector3 mulVec;
        mulVec.x = self.x * num;
        mulVec.y = self.y * num;
        mulVec.z = self.z * num;

        return mulVec;
      }

      Vector3 operator*(const float num, const Vector3& self)
      {
        return self * num;
      }

      Vector3 operator*(const Vector3& self, const Vector3& other)
      {
        Vector3 mulVec;
        mulVec.x = self.x * other.x;
        mulVec.y = self.y * other.y;
        mulVec.z = self.z * other.z;

        return mulVec;
      }

      Vector3 operator/(const Vector3& self, const float num)
      {
        if (num <= FLOAT_TOLERANCE && num >= -FLOAT_TOLERANCE)
        {
          return Vector3::PositiveInfinity;
        }

        Vector3 divVec;
        divVec.x = self.x / num;
        divVec.y = self.y / num;
        divVec.z = self.z / num;

        return divVec;
      }

      Vector3& Vector3::operator*=(const float num)
      {
        x *= num;
        y *= num;
        z *= num;

        return *this;
      }

      Vector3& Vector3::operator*=(const Vector3& other)
      {
        x *= other.x;
        y *= other.y;
        z *= other.z;

        return *this;
      }

      Vector3& Vector3::operator/=(const float num)
      {

          if (num <= FLOAT_TOLERANCE && num >= -FLOAT_TOLERANCE)
          {
            x = PositiveInfinity.x;
            y = PositiveInfinity.y;
            z = PositiveInfinity.z;
          }
          else
          {
            x /= num;
            y /= num;
            z /= num;
          }

          return *this;
  
      }

      bool operator==(const Vector3& self, const Vector3& other)
      {
        // Check x
        if ((self.x - other.x) > -FLOAT_TOLERANCE && (self.x - other.x) < FLOAT_TOLERANCE)
        {
          // Check y
          if ((self.y - other.y) > -FLOAT_TOLERANCE && (self.y - other.y) < FLOAT_TOLERANCE)
          {
            // Check z
            if ((self.z - other.z) > -FLOAT_TOLERANCE && (self.z - other.z) < FLOAT_TOLERANCE)
              return true;
          }
        }
        return false;
      }

      bool operator!=(const Vector3& self, const Vector3& other)
      {
        return !(self == other);
      }

      Vector3 Vector3::operator+() const
      {
        Vector3 positive(*this);

        return positive;
      }

      Vector3 Vector3::operator-() const
      {
        Vector3 negative(*this);

        return negative * (float)-1;
      }

      Vector3::operator Vector2() const noexcept
      {
        Vector2 cast(x, y);

        return cast;
      }

      Vector3::operator Vector3() const noexcept
      {
        Vector3 cast(*this);

        return cast;
      }

  #pragma endregion

    // Public method
    float Vector3::GetMagnitude() const
    {
      return sqrtf((powf(x, (float)2) + powf(y, (float)2)) + powf(z, (float)2));
    }

    Vector3 Vector3::GetNormalized() const
    {
      Vector3 normalized(*this);

      float tempMagnitude = GetMagnitude();

      if (tempMagnitude < FLOAT_TOLERANCE)
      {
        return Zero;
      }

      return normalized / tempMagnitude;
    }

    void Vector3::Normalize()
    {
      float tempMagnitude = GetMagnitude();

      if (tempMagnitude < FLOAT_TOLERANCE)
      {
        return;
      }

      x /= tempMagnitude;
      y /= tempMagnitude;
      z /= tempMagnitude;
    }
  }

}