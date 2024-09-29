/*

MRenderFramework
Author : MAI ZHICONG

Description : RenderFramework used by Game (Graphics API: DirectX12)

Update History: 2024/09/19 Create

Version : alpha_1.0.0

*/

#include <random>
#include <type_traits>
#include <Windows.h>
#include <cassert>

#define IsIntType(Type) { std::is_integral(Type)::value };
#define IsRealType(Type) { std::is_floating_point(Type)::value };

namespace MUtility
{
    namespace MRandom
    {
        template<typename T>
        struct UniformIntDistribution
        {
            std::uniform_int_distribution<T> Value;
        }; 

        template<typename T>
        struct UniformRealDistribution
        {
            std::uniform_real_distribution<T> Value;
        }; 

        template<typename T>
        struct DistributionTraits
        {
            using DistributionType = T;
        };

        template<>
        struct DistributionTraits<UINT8>
        {
            using DistributionType = UniformIntDistribution<UINT8>;
        };

        template<>
        struct DistributionTraits<INT8>
        {
            using DistributionType = UniformIntDistribution<INT8>;
        };

        template<>
        struct DistributionTraits<INT16>
        {
            using DistributionType = UniformIntDistribution<INT16>;
        };

        template<>
        struct DistributionTraits<UINT16>
        {
            using DistributionType = UniformIntDistribution<UINT16>;
        };

        template<>
        struct DistributionTraits<INT32>
        {
            using DistributionType = UniformIntDistribution<INT32>;
        };

        template<>
        struct DistributionTraits<UINT32>
        {
            using DistributionType = UniformIntDistribution<UINT32>;
        };

        template<>
        struct DistributionTraits<INT64>
        {
            using DistributionType = UniformIntDistribution<INT64>;
        };
        
        template<>
        struct DistributionTraits<UINT64>
        {
            using DistributionType = UniformIntDistribution<UINT64>;
        };

        template<>
        struct DistributionTraits<float>
        {
            using DistributionType = UniformRealDistribution<float>;
        };

        template<>
        struct DistributionTraits<double>
        {
            using DistributionType = UniformRealDistribution<double>;
        };

        template<typename T>
        class RandomGenerator
        {
            using Distribution = typename DistributionTraits<T>::DistributionType;
            
            public:
                RandomGenerator(T min, T max)
                {
                    // データ型がサポートされていないときエラーを吐き出す
                    static_assert((std::is_integral<T>::value) || (std::is_floating_point<T>::value), 
                                  "RandomGeneratorの型は整数や浮动小数点数にならなければなりません"
                                 );

                    if(min > max)
                    {
                        T temp = min;
                        min = max;
                        max = temp;
                    }

                    // 乱数生成シードを設定
                    std::random_device seedGenerator;
                    m_randomEngine.seed(seedGenerator());

                    // 乱数生成範囲を設定
                    decltype(m_distribution.Value.param()) distributionRange(min, max);
                    m_distribution.Value.param(distributionRange);

                    m_min = min;
                    m_max = max;
                }
                ~RandomGenerator()
                {

                }

            public:
                T GetMin() const 
                {
                    return m_min;
                }
                T GetMax() const
                {
                    return m_max;
                }

                T GenerateRandom()
                {
                    return m_distribution.Value(m_randomEngine);
                }
            private:
                RandomGenerator(const RandomGenerator&) = delete;
                RandomGenerator& operator=(const RandomGenerator&) = delete;

            private:
                std::default_random_engine m_randomEngine;
                Distribution m_distribution;
                T m_max;
                T m_min;
        };
    }
}
