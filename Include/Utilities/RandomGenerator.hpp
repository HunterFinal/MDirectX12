/*

MRenderFramework
Author : MAI ZHICONG

Description : RenderFramework used by Game (Graphics API: DirectX12)

Update History: 2024/09/19 Create

Version : alpha_1.0.0

*/

#ifndef M_RANDOM_GENERATOR
#define M_RANDOM_GENERATOR

#include <random>
#include <type_traits>
#include <cassert>

namespace MUtility
{
  namespace MRandom
  {
    template<bool IsInteger, typename T>
    struct DistributionTraits
    {
        
    };

    template<typename T>
    struct DistributionTraits<true, T>
    {
      using DistributionType = std::uniform_int_distribution<T>;
    };

    template<typename T>
    struct DistributionTraits<false, T>
    {
      using DistributionType = std::uniform_real_distribution<T>;
    };

    template<typename T>
    class RandomGenerator
    {
      // データ型がサポートされていない(整数型と浮動小数点数以外)ときエラーを吐き出す
      static_assert(
                    (std::is_integral<T>::value) || (std::is_floating_point<T>::value),
                    "RandomGeneratorの型は整数や浮動小数点数にならなければなりません"
                   );

      using RandomType = T;
      // 離散分布の型を指定
      using Distribution = typename DistributionTraits<std::is_integral_v<RandomType>, RandomType>::DistributionType;

      public:
          RandomGenerator(RandomType min, RandomType max)
          {
              if(min > max)
              {
                  RandomType temp = min;
                  min = max;
                  max = temp;
              }

              // 乱数生成シードを設定
              std::random_device seedGenerator;
              m_randomEngine.seed(seedGenerator());

              // 乱数生成範囲を設定
              ChangeGeneratorRange(min, max);

          }
          ~RandomGenerator()
          {

          }

          friend bool operator==(const RandomGenerator& lhs, const RandomGenerator& rhs)
          {
              return lhs.m_distribution == rhs.m_distribution;
          }

          friend bool operator!=(const RandomGenerator& lhs, const RandomGenerator& rhs)
          {
              return !(lhs == rhs);
          }

      public:
          /// <summary>
          /// 乱数ジェネレーターの最小値を返す
          /// </summary>
          /// <returns></returns>
          RandomType GetMin() const
          {
              return m_distribution.a();
          }
          /// <summary>
          /// 乱数ジェネレーターの最大値を返す
          /// </summary>
          /// <returns></returns>
          RandomType GetMax() const
          {
              return m_distribution.b();
          }
          /// <summary>
          /// 乱数を生成
          /// </summary>
          /// <returns></returns>
          RandomType GenerateRandom()
          {
              return m_distribution(m_randomEngine);
          }

          /// <summary>
          /// 乱数の生成範囲を変更
          /// </summary>
          void ChangeGeneratorRange(RandomType min, RandomType max)
          {
              decltype(m_distribution.param()) distributionRange(min, max);
              m_distribution.param(distributionRange);
          }

      private:
          RandomGenerator(const RandomGenerator&) = delete;
          RandomGenerator& operator=(const RandomGenerator&) = delete;

      private:
          std::default_random_engine m_randomEngine;
          Distribution m_distribution;
    };
  }
}

#endif
