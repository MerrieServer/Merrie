#ifndef MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_CONTAINERS_HPP
#   error "Include Containers.hpp instead"
#endif

#ifndef MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_CONTAINERS_TCC
#define MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_CONTAINERS_TCC

namespace Merrie {

    template<typename K, typename V>
    std::optional<V> FindInMap(const std::map<K, V>& map, const K& key) {
        const auto& iterator = map.find(key);

        return iterator == map.end()
               ? std::nullopt
               : std::make_optional(iterator->second);
    }

    template<typename V>
    bool VectorContains(const std::vector<V>& vector, const V& value)
    {
        return std::find(begin(vector), end(vector), value) != end(vector);
    }
}


#endif //MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_CONTAINERS_TCC
