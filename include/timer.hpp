#include <chrono>
#include <iostream>
namespace ch = std::chrono;

class GlobalTimer
{
    using clock = ch::system_clock;
    using ns = ch::nanoseconds;
    using ms = ch::milliseconds;
    using s = ch::seconds;

public:
    GlobalTimer() = default;
    void set()
    {
        start = clock::now();
    }
    /**
     * @brief get time elapsed since last set()
     *
     * @return in ms
     */
    auto diff()
    {
        return ch::duration_cast<ns>(clock::now() - start);
    }
    auto diff_s()
    {
        return ch::duration_cast<s>(clock::now() - start);
    }
    auto diff_ms()
    {
        return ch::duration_cast<ms>(clock::now() - start);
    }
    void print_s(std::ostream &out)
    {
        out.precision(3);
        out << "duration: " << std::fixed
            << ch::duration_cast<s>(diff()).count() << " seconds" << std::endl;
    }
    void print_ms(std::ostream &out)
    {
        out.precision(3);
        out << "duration: " << std::fixed
            << ch::duration_cast<ms>(diff()).count() << " miliseconds"
            << std::endl;
    }
    void print_ns(std::ostream &out)
    {
        out << "duration: " << ch::duration_cast<ns>(diff()).count()
            << " nanoseconds" << std::endl;
    }

private:
    clock::time_point start;
};

class Timer
{
    using clock = ch::steady_clock;
    using ns = ch::nanoseconds;
    using ms = ch::milliseconds;
    using s = ch::seconds;

public:
    Timer() = default;
    void set()
    {
        start = clock::now();
    }
    /**
     * @brief get time elapsed since last set()
     *
     * @return in ms
     */
    auto diff()
    {
        return ch::duration_cast<ns>(clock::now() - start);
    }
    auto diff_s()
    {
        return ch::duration_cast<s>(clock::now() - start);
    }
    auto diff_ms()
    {
        return ch::duration_cast<ms>(clock::now() - start);
    }
    void print_s(std::ostream &out)
    {
        out.precision(3);
        out << "duration: " << std::fixed
            << ch::duration_cast<s>(diff()).count() << " seconds" << std::endl;
    }
    void print_ms(std::ostream &out)
    {
        out.precision(3);
        out << "duration: " << std::fixed
            << ch::duration_cast<ms>(diff()).count() << " miliseconds"
            << std::endl;
    }
    void print_ns(std::ostream &out)
    {
        out << "duration: " << ch::duration_cast<ns>(diff()).count()
            << " nanoseconds" << std::endl;
    }

private:
    clock::time_point start;
};
