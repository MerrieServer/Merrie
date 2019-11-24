#ifndef MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_TIME_HPP
#define MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_TIME_HPP

#include "Commons.hpp"
#include <chrono>

namespace Merrie {

    /**
     * The default clock used by the Proxy for internal time calculations
     */
    using DefaultClock = std::chrono::high_resolution_clock;

    /**
    * Gets the time_point in future, relative to the current time of the DefaultClock
    * @tparam Duration unit of time to be used (i.e. std::chrono::seconds)
    * @param amount amount of time, in the unit as specified in Duration
    * @return the time point in the future, as specified by the amount and Duration
    */
    template<class Duration>
    inline DefaultClock::time_point PointInFuture(size_t amount)
    {
        return DefaultClock::now() + Duration(amount);
    }

    /**
     * Checks whether the given time point is past according to the DefaultClock
     *
     * @param timePoint time point to be checked
     * @tparam TimePoint type of time point
     * @return true if the time point is in the past
     */
    template<class TimePoint>
    inline bool IsPast(TimePoint timePoint)
    {
        return timePoint < DefaultClock::now();
    }

} // namespace Merrie

#endif //MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_TIME_HPP
