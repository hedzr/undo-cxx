// undo_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/14.
//

#ifndef UNDO_CXX_UNDO_DBG_HH
#define UNDO_CXX_UNDO_DBG_HH

#include <string>
#include <vector>

namespace undo_cxx { namespace debug {

#if __cplusplus < 201402
    template<class T>
    std::string
    type_name() {
        typedef typename std::remove_reference<T>::type TR;
        std::unique_ptr<char, void (*)(void *)> own(
#ifndef _MSC_VER
                abi::__cxa_demangle(typeid(TR).name(), nullptr,
                                    nullptr, nullptr),
#else
                nullptr,
#endif
                std::free);
        std::string r = own != nullptr ? own.get() : typeid(TR).name();
        if (std::is_const<TR>::value)
            r += " const";
        if (std::is_volatile<TR>::value)
            r += " volatile";
        if (std::is_lvalue_reference<T>::value)
            r += "&";
        else if (std::is_rvalue_reference<T>::value)
            r += "&&";
#ifndef _MSC_VER
        return r;
#else
        const char *const pr1 = "struct ";
        auto ps1 = r.find(pr1);
        auto st1 = (ps1 != std::string::npos ? ps1 + sizeof(pr1) - 1 : 0);
        auto name1 = r.substr(st1);
        const char *const pr2 = "class ";
        auto ps2 = name1.find(pr2);
        auto st2 = (ps2 != std::string::npos ? ps2 + sizeof(pr2) - 1 : 0);
        auto name2 = name1.substr(st2);
        const char *const pr3 = "union ";
        auto ps3 = name2.find(pr3);
        auto st3 = (ps3 != std::string::npos ? ps3 + sizeof(pr3) - 1 : 0);
        auto name3 = name2.substr(st3);
        return name3;
#endif
    }

    /**
     * @brief remove the scoped prefix (before '::')
     * @tparam T 
     * @return 
     */
    template<typename T>
    constexpr auto short_type_name() -> std::string_view {
        const auto value = type_name<T>();
        const auto end = value.rfind("::");
        return std::string_view{value.data() + (end != std::string_view::npos ? end + 2 : 0)};
    }

#else
    template<typename T>
    constexpr auto type_name_1() noexcept {
        // std::string_view name, prefix, suffix;
#ifdef __clang__
        constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
        constexpr auto prefix = std::string_view{"auto type_name() [T = "};
        constexpr auto suffix = std::string_view{"]"};
#elif defined(__GNUC__)
        constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
        constexpr auto prefix = std::string_view{"constexpr auto type_name() [with T = "};
        constexpr auto suffix = std::string_view{"]"};
#elif defined(_MSC_VER)
        constexpr auto function = std::string_view{__FUNCSIG__};
        constexpr auto prefix = std::string_view{"auto __cdecl type_name<"};
        constexpr auto suffix = std::string_view{">(void) noexcept"};
#else
        constexpr auto function = std::string_view{"Error: unsupported compiler"};
        constexpr auto prefix = std::string_view{""};
        constexpr auto suffix = std::string_view{""};
#endif

        //name.remove_suffix(suffix.size());
        //name.remove_prefix(prefix.size());

        constexpr auto start = function.find(prefix) + prefix.size();
        constexpr auto end = function.rfind(suffix);
        return function.substr(start, (end - start));
    }

    template<std::size_t... Indexes>
    constexpr auto substring_as_array(std::string_view str, std::index_sequence<Indexes...>) {
        return std::array{str[Indexes]..., '\n'};
    }

    template<typename T>
    constexpr auto type_name_array() {
#if defined(__clang__)
        constexpr auto prefix = std::string_view{"[T = "};
        constexpr auto suffix = std::string_view{"]"};
        constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(__GNUC__)
        constexpr auto prefix = std::string_view{"with T = "};
        constexpr auto suffix = std::string_view{"]"};
        constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(_MSC_VER)
        constexpr auto prefix = std::string_view{"type_name_array<"};
        constexpr auto suffix = std::string_view{">(void)"};
        constexpr auto function = std::string_view{__FUNCSIG__};
#else
#error Unsupported compiler
#endif

        constexpr auto start = function.find(prefix) + prefix.size();
        constexpr auto end = function.rfind(suffix);

        static_assert(start < end);

        constexpr auto name = function.substr(start, (end - start));
#if !defined(_MSC_VER)
        // return substring_as_array(name, std::make_index_sequence<name.size()>{});
        return name;
#else
        constexpr auto pr1 = std::string_view{"struct "};
        constexpr auto ps1 = r.find(pr1);
        constexpr auto st1 = (ps1 >= 0 ? ps1 : -pr1.size()) + pr1.size();
        constexpr auto name1 = r.substr(st1);
        constexpr auto pr2 = std::string_view{"class "};
        constexpr auto ps2 = name1.find(pr2);
        constexpr auto st2 = (ps2 >= 0 ? ps2 : -pr2.size()) + pr2.size();
        constexpr auto name2 = name1.substr(st2);
        constexpr auto pr3 = std::string_view{"union "};
        constexpr auto ps3 = name2.find(pr3);
        constexpr auto st3 = (ps3 >= 0 ? ps3 : -pr3.size()) + pr3.size();
        constexpr auto name3 = name2.substr(st3);
        return name3;
#endif
    }

    template<typename T>
    struct type_name_holder {
        static inline constexpr auto value = type_name_array<T>();
    };

    /**
     * @brief return the literal of a datatype in constexpr.
     * @tparam T the datatype
     * @return std::string_view
     * @note the returning string is a string_view, make a copy before print it:
     * 
     * 1. use std::ostream directly:
     * @code{c++}
     * std::cout << hicc::debug::type_name&lt;std::string>() << '\n';
     * @endcode
     * 
     * 2. wrap the string_view into a std::string:
     * @code{c++}
     * std::cout &lt;&lt; std::string(hicc::debug::type_name&lt;std::string>()) &lt;&lt; '\n';
     * printf(">>> %s\n", std::string(hicc::debug::type_name&lt;std::string>()).c_str());
     * @endcode
     * 
     */
    template<typename T>
    constexpr auto type_name() -> std::string_view {
        constexpr auto &value = type_name_holder<T>::value;
        return std::string_view{value.data(), value.size()};
    }

    /**
     * @brief remove the scoped prefix (before '::')
     * @tparam T 
     * @return 
     */
    template<typename T>
    constexpr auto short_type_name() -> std::string_view {
        constexpr auto &value = type_name_holder<T>::value;
        constexpr auto end = value.rfind("::");
        return std::string_view{value.data() + (end != std::string_view::npos ? end + 2 : 0)};
    }

    // https://bitwizeshift.github.io/posts/2021/03/09/getting-an-unmangled-type-name-at-compile-time/
#endif


    // to detect the type of a lambda function, following:
    //   https://stackoverflow.com/a/7943736/6375060

    template<class>
    struct mem_type;

    template<class C, class T>
    struct mem_type<T C::*> {
        typedef T type;
    };

    template<class T>
    struct lambda_func_type {
        typedef typename mem_type<decltype(&T::operator())>::type type;
    };

#if 0
    void main_lambda_compare() {
        auto l = [](int i) { return long(i); };
        typedef lambda_func_type<decltype(l)>::type T;
        static_assert(std::is_same<T, long(int) const>::value, "ok");
    }
#endif

}} // namespace undo_cxx::debug

#endif //UNDO_CXX_UNDO_DBG_HH
