#pragma once

#include "format.hpp"

namespace utils
{
    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    T pow2(const T &value)
    {
        return value * value;
    }

    template <typename T>
    T pow3(const T &value)
    {
        return value * pow2(value);
    }

    template <typename _Iterator>
    using _iterator_category_t = typename std::iterator_traits<_Iterator>::iterator_category;

    template <typename _InputIterator>
    using is_input_iterator_t = std::enable_if_t<std::is_convertible_v<_iterator_category_t<_InputIterator>, std::input_iterator_tag>, bool>;

    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    T sqrt(const T &value)
    {
        if (value < 0)
        {
            throw std::out_of_range(format("Attempted to calculate square root of %s < 0", std::to_string(value).c_str()));
        }

        if (value == 0)
        {
            return 0;
        }

        T low = 0, high = std::max(static_cast<T>(1), value), accuracy = 1;
        if constexpr (std::is_floating_point_v<T>)
        {
            accuracy = 1.0e-7;
        }

        while (high - low > accuracy)
        {
            double mid = (low + high) / 2;
            if (mid * mid < value)
            {
                low = mid;
            }
            else
            {
                high = mid;
            }
        }

        return high;
    }

    template <typename T>
    T distance(const T &dx, const T &dy)
    {
        return sqrt(pow2(dx) + pow2(dy));
    }

    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    T abs(const T &value)
    {
        return value > 0 ? value : -value;
    }

    template <typename T>
    bool approximate(const T &first, const T &second)
    {
        return first == second;
    }

    template <>
    bool approximate(const double &first, const double &second)
    {
        return abs(first - second) < 1.0e-6;
    }

    template <>
    bool approximate(const float &first, const float &second)
    {
        return abs(first - second) < 1.0e-6;
    }

    template <typename T>
    bool approximate(const std::vector<T> &first, const std::vector<T> &second)
    {
        return std::equal(
            first.begin(), first.end(),
            second.begin(), second.end(),
            [](const T &first, const T &second)
            {
                return approximate(first, second);
            });
    }

    /**
     * @brief Get the size of the console window using
     * [`GetConsoleScreenBufferInfo`](https://learn.microsoft.com/en-us/windows/console/getconsolescreenbufferinfo)
     * or [`ioctl`](https://man7.org/linux/man-pages/man2/ioctl.2.html).
     *
     * @note In case it is not possible to get the console size, `std::runtime_error` is thrown.
     * @return The number of columns and rows, respectively.
     */
    std::pair<unsigned short, unsigned short> get_console_size()
    {
#if defined(WIN32)
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info))
        {
            throw std::runtime_error("GetConsoleScreenBufferInfo ERROR");
        }

        SHORT columns = info.srWindow.Right - info.srWindow.Left + 1,
              rows = info.srWindow.Bottom - info.srWindow.Top + 1;

        return std::make_pair(columns, rows);

#elif defined(__linux__)
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
        {
            throw std::runtime_error("ioctl ERROR");
        }

        return std::make_pair(w.ws_col, w.ws_row);
#else
        throw std::runtime_error("Unsupported platform");
#endif
    }
}
