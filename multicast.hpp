#ifndef UTIL_MULTIFUNCTION_HPP
#define UTIL_MULTIFUNCTION_HPP

#include <functional>
#include <utility>
#include <vector>

namespace util {

namespace detail {

    template <typename R, typename... Args>
    struct multifunction_traits {
        typedef R return_type;
    };

    template <typename R, typename Arg>
    struct multifunction_traits<R, Arg> {
        typedef R return_type;
        typedef Arg argument_type;
    };

    template <typename R, typename Arg1, typename Arg2>
    struct multifunction_traits<R, Arg1, Arg2> {
        typedef R return_type;
        typedef Arg1 first_argument_type;
        typedef Arg2 second_argument_type;
    };

    template <typename R, typename... Args>
    struct call_helper {
        template <typename... Ts>
        static R call(
            std::vector<std::function<R(Args...)>> const& listeners,
            Ts&&... args
        ) {
            R ret;
            for (auto listener : listeners) {
                ret = listener(std::forward<Ts>(args)...);
            }
            return ret;
        }
    };

    template <typename... Args>
    struct call_helper<void, Args...> {
        template <typename... Ts>
        static void call(
            std::vector<std::function<void(Args...)>> const& listeners,
            Ts&&... args
        ) {
            for (auto listener : listeners) {
                listener(std::forward<Ts>(args)...);
            }
        }
    };

} // namespace detail

template <typename R, typename... Args>
class multifunction;

template <typename R, typename... Args>
class multifunction<R(Args...)> : public detail::multifunction_traits<R, Args...> {
public:

    // Uniquely identifies a listener in this multifunction.
    // This is an opaque type, meaning it can only be created and accessed
    // from within this class. The outside world can only store them.
    class handle {
        friend class multifunction;

        handle(std::size_t id) noexcept : id(static_cast<unsigned>(id)) { }

        // We assume that numeric_limits<unsigned>::max() handles are enough.
        unsigned id;
    };

    multifunction() = default;
    multifunction(multifunction const&) = default;
    multifunction(multifunction&&) = default;
    multifunction& operator =(multifunction const&) = default;
    multifunction& operator =(multifunction&&) = default;
    ~multifunction() = default;

    template <typename F>
    handle operator +=(F listener) {
        listeners.push_back(listener);
        handle_lookup.push_back(listeners.size() - 1);
        return handle{handle_lookup.size() - 1};
    }

    void operator -=(handle handle) {
        auto i = handle_lookup[handle.id];

        if (i == NIL) return;

        // Adjust handle_lookup positions which have shifted.
        for (auto& handle_index : handle_lookup) {
            if (handle_index > i and handle_index != NIL) {
                --handle_index;
            }
        }

        listeners.erase(listeners.begin() + i);
        handle_lookup[handle.id] = NIL;
    }

    template <typename... Ts>
    R operator ()(Ts&&... args) const {
        return detail::call_helper<R, Args...>::call(listeners, std::forward<Ts>(args)...);
    }
    
private:

    std::vector<std::function<R(Args...)>> listeners;
    std::vector<std::size_t> handle_lookup;
    static constexpr std::size_t NIL = -1;
};

} // namespace util

#endif // ndef UTIL_MULTIFUNCTION_HPP
