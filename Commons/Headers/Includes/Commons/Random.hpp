#ifndef MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_RANDOM_HPP
#define MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_RANDOM_HPP

#include "Commons.hpp"
#include <random>

namespace Merrie {

    /**
     * Gets the common default std::random_device instance.
     */
    [[nodiscard]] std::random_device& GetCommonRandomDevice();

    /**
     * Gets the common default std::mt19937_64instance.
     */
    [[nodiscard]] std::mt19937_64& GetCommonMt19937();

    /**
     * A function that returns a randomly generated number when called.
     */
    template<typename Number>
    using RandomNumberGenerator = std::function<Number()>;

    /**
     * Creates a new RandomNumberGenerator
     *
     * @tparam Number type of the numbers to generate
     * @param minimum minimum number to generate by the generator (inclusive)
     * @param maximum maximum number to generate by the generator (exclusive)
     * @param twister the mersenne twister to use
     * @return the newly created RandomNumberGenerator
     */
    template<typename Number>
    RandomNumberGenerator<Number> CreateRandomNumberGenerator(Number minimum, Number maximum, std::mt19937_64 twister);

    /**
     * Creates a new RandomNumberGenerator using the GetCommonMt19937() as the mersenne twister
     *
     * @tparam Number type of the numbers to generate
     * @param minimum minimum number to generate by the generator (inclusive)
     * @param maximum maximum number to generate by the generator (exclusive)
     * @return the newly created RandomNumberGenerator
     */
    template<typename Number>
    RandomNumberGenerator<Number> CreateRandomNumberGenerator(Number minimum, Number maximum);

    /**
     * Creates a new RandomNumberGenerator that can generate any number that can fit in Number.
     *
     * @tparam Number type of the numbers to generate
     * @param twister the mersenne twister to use
     * @return the newly created RandomNumberGenerator
     */
    template<typename Number>
    RandomNumberGenerator<Number> CreateRandomNumberGenerator(std::mt19937_64 twister);

    /**
     * Creates a new RandomNumberGenerator that can generate any number that can fit in Number using the GetCommonMt19937() as the mersenne twister.
     *
     * @tparam Number type of the numbers to generate
     * @return the newly created RandomNumberGenerator
     */
    template<typename Number>
    RandomNumberGenerator<Number> CreateRandomNumberGenerator();
}

#include "Random.tcc"
#endif //MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_RANDOM_HPP
