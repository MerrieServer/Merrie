#ifndef MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_CONTAINERS_HPP
#   error "Include Containers.hpp instead"
#endif

#ifndef MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_CONTAINERS_TCC
#define MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_CONTAINERS_TCC

namespace Merrie {

    template<typename K, typename V>
    inline std::optional<V> FindInMap(const std::map<K, V>& map, const K& key) {
        const auto& iterator = map.find(key);

        return iterator == map.end()
               ? std::nullopt
               : std::make_optional(iterator->second);
    }

    template<typename C, typename V>
    inline bool Contains(const C& container, const V& value) {
        return std::find(begin(container), end(container), value) != end(container);
    }

    template<typename C, typename Predicate>
    inline void RemoveIf(C& container, Predicate predicate) {
        container.erase(std::remove_if(begin(container), end(container), predicate), end(container));
    }
}


#endif //MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_CONTAINERS_TCC
