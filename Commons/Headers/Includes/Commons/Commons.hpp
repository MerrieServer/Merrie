#ifndef MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_COMMONS_HPP
#define MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_COMMONS_HPP

// Common includes
#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <cstdint>

#ifdef M_PLATFORM_WINDOWS
#   include <winsdkver.h>
#endif

namespace Merrie {
    using namespace std::string_literals;

    // ================================================================================
    // = Copy & Move semantics                                                        =
    // ================================================================================

    /**
     * Declares the given type as non-copyable by explicitly deleting  the copy constructor and copy assignment operator.
     *
     * Can only be used inside a class declaration.
     */
    #define NON_COPYABLE(NAME)                                  \
        NAME(const NAME& rhs) = delete;                         \
        NAME& operator=(const NAME& rhs) = delete

    /**
     * Declares the given type as non-moveable by explicitly deleting the move constructor and move assignment operator.
     *
     * Can only be used inside a class declaration.
     */
    #define NON_MOVEABLE(NAME)                                  \
        NAME(NAME&& rhs) = delete;                              \
        NAME& operator=(NAME&& rhs) = delete

    /**
     * Declares the given type is trivially copyable and the compiler must generate the copy constructor and copy assignment operator.
     *
     * Can only be used inside a class declaration.
     */
    #define TRIVIALLY_COPYABLE(NAME)                             \
        NAME(const NAME& rhs) = default;                         \
        NAME& operator=(const NAME& rhs) = default

    /**
     * Declares the given type is trivially moveable and the compiler must generate the move constructor and move assignment operator.
     *
     * Can only be used inside a class declaration.
     */
    #define TRIVIALLY_MOVEABLE(NAME)                             \
        NAME(NAME&& rhs) noexcept = default;                     \
        NAME& operator=(NAME&& rhs) noexcept = default

    // ================================================================================
    // = Initializers                                                                 =
    // ================================================================================
    #define M_INITIALIZER(functionName)                                                                                   \
        static_assert(noexcept(functionName()), "M_INITIALIZER function must be explicitly marked as noexcept");          \
                                                                                                                          \
        class functionName##_initializer {                                                                                \
            public:                                                                                                       \
                inline functionName##_initializer() noexcept {                                                            \
                    functionName();                                                                                       \
                }                                                                                                         \
        };                                                                                                                \
                                                                                                                          \
        static const functionName##_initializer g_##functionName##_initializer

    // ================================================================================
    // = Simple exceptions                                                            =
    // ================================================================================

    /**
     * Base class of any exception class.
     */
    class BaseException : public std::exception {
        public:
            [[nodiscard]] const char* what() const noexcept override { return m_what.data(); }

        protected:
            explicit BaseException(std::string what) noexcept : m_what(std::move(what)) {}

        private:
            const std::string m_what;
    };

    /**
     * Declares a new exception with the given name and with a specified base class.
     */
    #define M_DECLARE_EXCEPTION_EX(NAME, BASE_CLASS)                                                    \
        class NAME : public BASE_CLASS {                                                                \
        public:                                                                                         \
            explicit NAME(std::string what) noexcept : BASE_CLASS(std::move(what)) {}                   \
        }

    /**
     * Declares a new exception with the given name and BaseException as a base class
     */
    #define M_DECLARE_EXCEPTION(NAME) \
        M_DECLARE_EXCEPTION_EX(NAME, Merrie::BaseException)

    // ================================================================================
    // = Assertions                                                                   =
    // ================================================================================

    // Assertions are enabled by default
    #if !defined(M_ASSERTIONS_ENABLED) && !defined(M_ASSERTIONS_DISABLED)
    #   define M_ASSERTIONS_ENABLED
    #endif

    /**
     * Thrown when M_ASSERT's conditions fails and assertions are enabled.
     */
    M_DECLARE_EXCEPTION(AssertionError);

    /**
     * Fails with AssertionError and the specified message
     */
    [[noreturn]] inline void M_FAIL(std::string message) {
        throw Merrie::AssertionError(std::move(message));
    }

    /**
     * \def M_ASSERT(condition, message)
     *
     * If M_ASSERTIONS_DISABLED is specified this macro does nothing.
     *
     * Checks whether or not the specified condition is true.
     * If it is true nothing is done.
     * If it is false Merrie::AssertionError is thrown.
     *
     * No side-effects expression should be specified as condition since it may never be evaluated if the assertions are disabled
     */
    #ifdef M_ASSERTIONS_ENABLED
    #   define M_ASSERT(condition, message) do { if (!(condition)) Merrie::M_FAIL(message); } while(0)
    #else
    #   define M_ASSERT(condition, message) do {} while(0)
    #endif

}  // namespace Merrie

#endif //MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_LOGGING_HPP
