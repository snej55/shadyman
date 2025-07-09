#ifndef UTIL_H
#define UTIL_H

#include <array>
#include <vector>
#include <cstdlib>
#include <cmath>

#include "./vec2.hpp"

namespace Util {
    template <typename T, int N>
    inline bool elementIn(const T& element, const T arr[N])
    {
        for (std::size_t i{0}; i < N; ++i)
        {
            if (element == arr[i])
            {
                return true;
            }
        }
        return false;
    }

    template <typename T, int N>
    inline T pickRandom(const T arr[N])
    {
        return arr[static_cast<std::size_t>(std::rand() % N)];
    }

    inline float random()
    {
        return static_cast<float>((float)std::rand() / (RAND_MAX));
    }

    template <typename T>
    inline float distance(vec2<T> vec1, vec2<T> vec2)
    {
        return std::sqrt(std::pow(vec1.y - vec2.y, 2) + std::pow(vec1.x - vec2.x, 2));
    }

    template <typename T>
    inline void swap(T** val1, T** val2)
    {
        T* temp {*val1}; // create temp for value of val1
        *val1 = *val2; // swap addresses
        *val2 = temp;
    }

    template <typename type>
    std::vector<int> get_water_indices(std::vector<type> vertices)
    {
        int T{static_cast<int>(vertices.size())};
        int H{static_cast<int>((T + 1) / 2)};
        int TQ{static_cast<int>((T - 1) / 2)};
        std::vector<int> indices;
        indices.resize(TQ * 6);

        for (int c{0}; c < TQ; ++c)
        {
            int qi {c * 6}; // quad index
            indices[qi] = c;
            indices[qi + 1] = c + 1;
            indices[qi + 2] = H + c;
            indices[qi + 3] = H + c;
            indices[qi + 4] = H + c + 1;
            indices[qi + 5] = c + 1;
        }

        return indices;
    }

    template <typename T>
    void printVector(const std::vector<T>& vec)
    {
        const std::size_t size{vec.size()};
        std::cout << "{";
        for (std::size_t i{0}; i < size; ++i)
        {
            std::cout << vec[i];
            if (i < size - 1)
            {
                std::cout << ", ";
            }
        }
        std::cout << "}\n";
    }
}

#endif