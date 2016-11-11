#ifndef PHI_UTILS_TIMER_H
#define PHI_UTILS_TIMER_H
#include <chrono>

namespace phi {

using namespace std::chrono;
class Timer {
    using clock_type = steady_clock;

    time_point<clock_type> m_beg;
    time_point<clock_type> m_end;
    unsigned m_runs;
    long double m_interval_max;
    long double m_interval_sum;

public:
    Timer()
            : m_beg(),
              m_end(),
              m_runs(0),
              m_interval_max(0),
              m_interval_sum(0) {}

    inline void Begin() {
        m_beg = clock_type::now();
    }

    inline void End() {
        m_end = clock_type::now();
        ++m_runs;
        auto dt = ElapsedTimeUs<long double>();
        m_interval_sum += dt;
        m_interval_max = std::max(m_interval_max, dt);
    }

    template <typename T>
    T ElapsedTimeUs() {
        return duration_cast<microseconds>(m_end - m_beg).count();
    }

    template <typename T>
    T AvgUs() {
        return static_cast<T>(m_interval_sum / m_runs);
    }

    template <typename T>
    T MaxUs() {
        return static_cast<T>(m_interval_max);
    }
};

} // namespace phi

#endif
