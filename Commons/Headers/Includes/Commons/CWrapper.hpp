#ifndef MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_CWRAPPER_HPP
#define MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_CWRAPPER_HPP

#include "Commons.hpp"

namespace Merrie {

    /**
     * Pointer to a free-like function, taking pointer to T as an argument.
     */
    template<typename T>
    using SpecializedFreeFunction = std::add_pointer_t<void(T*)>;

    /**
     * Pointer to a free-like function, taking void* as an argument.
     */
    using FreeFunction = SpecializedFreeFunction<void>;

    /**
     * An unique-ptr that uses a free-like function as a deleter.
     * Its primary use is a RAII wrapper for raw C structs.
     */
    template<typename T, typename Deleter = FreeFunction>
    using UniqueWrapper = std::unique_ptr<T, Deleter>;

} // namespace Merrie

#endif //MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_CWRAPPER_HPP
