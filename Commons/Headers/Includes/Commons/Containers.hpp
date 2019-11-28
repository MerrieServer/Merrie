#ifndef MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_CONTAINERS_HPP
#define MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_CONTAINERS_HPP

#include "Commons.hpp"
#include <optional>

namespace Merrie {

    /**
     * Finds an element in a map by the given key.
     *
     * @tparam K type of the key elemenets in map
     * @tparam V type of the value elements in map
     * @param map map to be searched
     * @param key key to search for
     * @return the found element or std::nullopt if none found
     */
    template<typename K, typename V>
    std::optional<V> FindInMap(const std::map<K, V>& map, const K& key);

    /**
     * Checks whether or not the given vector contains the given value
     * @tparam V type of the values in the vector
     * @param vector the vector to search in
     * @param value the value to search for
     * @return whether or not the value is contained in the vector
     */
    template<typename V>
    bool VectorContains(const std::vector<V>& vector, const V& value);
}

#include "Containers.tcc"
#endif //MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_CONTAINERS_HPP
