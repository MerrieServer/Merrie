#ifndef MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_RANDOM_HPP
#   error "Include Random.hpp instead"
#endif

#ifndef MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_RANDOM_TCC
#define MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_RANDOM_TCC

namespace Merrie {

    template<typename Number>
    RandomNumberGenerator <Number> CreateRandomNumberGenerator(Number minimum, Number maximum, std::mt19937 twister) {
        static_assert(std::is_arithmetic_v<Number>, "Number must be arithmetic");

        using Distribution = std::conditional_t<
                std::is_integral_v<Number>,
                std::uniform_int_distribution<Number>,
                std::uniform_real_distribution<Number>
        >;

        return [distribution = Distribution(minimum, maximum), twister = std::move(twister)]() mutable -> Number {
            return distribution(twister);
        };
    }

    template<typename Number>
    RandomNumberGenerator <Number> CreateRandomNumberGenerator(Number minimum, Number maximum) {
        return CreateRandomNumberGenerator(minimum, maximum, GetCommonMt19937());
    }

    template<typename Number>
    RandomNumberGenerator <Number> CreateRandomNumberGenerator(std::mt19937 twister) {
        return CreateRandomNumberGenerator(std::numeric_limits<Number>::min(), std::numeric_limits<Number>::max(), std::move(twister));
    }

    template<typename Number>
    RandomNumberGenerator <Number> CreateRandomNumberGenerator() {
        return CreateRandomNumberGenerator(std::numeric_limits<Number>::min(), std::numeric_limits<Number>::max(), GetCommonMt19937());
    }
}

#endif //MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_RANDOM_TCC
