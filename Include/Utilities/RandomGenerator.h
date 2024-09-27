/*

MRenderFramework
Author : MAI ZHICONG

Description : RenderFramework used by Game (Graphics API: DirectX12)

Update History: 2024/09/19 Create

Version : alpha_1.0.0

*/

#include <random>

namespace MUtility
{
    namespace MRandom
    {
        template<typename T>
        class RandomGenerator
        {
            public:
                RandomGenerator();
                ~RandomGenerator();
            private:
                RandomGenerator(const RandomGenerator&) = delete;
                RandomGenerator& operator=(const RandomGenerator&) = delete;

            private:
                std::default_random_engine m_randomEngine;
                std::uniform_int_distribution<T> m_destribution;
                T m_max;
                T m_min;
        };
    }
}
