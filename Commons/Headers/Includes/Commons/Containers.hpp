#ifndef MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_CONTAINERS_HPP
#define MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_CONTAINERS_HPP

#include "Commons.hpp"
#include <optional>
#include <vector>

namespace Merrie {

    /**
     * Finds an element in a map by the given key.
     *
     * @tparam K type of the key elements in map
     * @tparam V type of the value elements in map
     * @param map map to be searched
     * @param key key to search for
     * @return the found element or std::nullopt if none found
     */
    template<typename K, typename V>
    inline std::optional<V> FindInMap(const std::map<K, V>& map, const K& key);

    /**
     * Checks whether or not the given container contains the given value
     * @tparam C type of the container
     * @tparam V type of the values in the container
     * @param container the container to search in
     * @param value the value to search for
     * @return whether or not the value is contained in the container
     */
    template<typename C, typename V>
    inline bool Contains(const C& container, const V& value);

    /**
     * Removes all values from a container that match the given predicate.
     * @tparam C type of the container
     * @tparam Predicate type of the predicate
     * @param container container to remove values from
     * @param predicate predicate to check against
     */
    template<typename C, typename Predicate>
    inline void RemoveIf(C& container, Predicate predicate);

}

#include "Containers.tcc"
#endif //MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_CONTAINERS_HPP
