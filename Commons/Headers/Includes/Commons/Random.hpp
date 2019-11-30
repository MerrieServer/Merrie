#ifndef MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_RANDOM_HPP
#define MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_RANDOM_HPP

#include "Commons.hpp"
#include <random>

namespace Merrie {

    [[nodiscard]] std::random_device& GetCommonRandomDevice();

    [[nodiscard]] std::mt19937& GetCommonMt19937();

    template<typename Number>
    using RandomNumberGenerator = std::function<Number()>;

    template<typename Number>
    RandomNumberGenerator<Number> CreateRandomNumberGenerator(Number minimum, Number maximum, std::mt19937 twister = GetCommonMt19937());

    template<typename Number>
    RandomNumberGenerator<Number> CreateRandomNumberGenerator(Number minimum, Number maximum);

    template<typename Number>
    RandomNumberGenerator<Number> CreateRandomNumberGenerator(std::mt19937 twister = GetCommonMt19937());

    template<typename Number>
    RandomNumberGenerator<Number> CreateRandomNumberGenerator();
}

#include "Random.tcc"
#endif //MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_RANDOM_HPP
