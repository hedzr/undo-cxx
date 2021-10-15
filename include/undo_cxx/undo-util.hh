// undo_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/14.
//

#ifndef UNDO_CXX_UNDO_UTIL_HH
#define UNDO_CXX_UNDO_UTIL_HH

#include "undo-def.hh"

#include "undo-dbg.hh"

#include <string>
#include <string_view>
#include <tuple>

#include <memory>


#if !defined(__ID_SYSTEM_DEFINED)
#define __ID_SYSTEM_DEFINED
#include <string_view>
#include <type_traits>
#include <utility>
// ------------------- id
namespace undo_cxx {

#if defined(_MSC_VER)
    using id_type = std::string; // or std::string_view
#else
    using id_type = std::string_view;
#endif

    namespace detail {
        template<class T, bool = std::is_enum<T>::value>
        struct __enum_id_gen : public std::unary_function<T, id_type> {
            id_type operator()(T) const {
                // typedef typename std::underlying_type<T>::type type;
                constexpr id_type v = debug::type_name<T>();
                constexpr auto end = v.find('<');
                if (end != v.npos)
                    return v.substr(0, end);
                return v;
            }
        };
    } // namespace detail

    template<typename T>
    struct id_gen : public detail::__enum_id_gen<T> {};

    template<typename T>
    constexpr auto id_name() -> id_type {
        constexpr id_type v = debug::type_name<T>();
        constexpr auto end = v.find('<');
        if (end != v.npos)
            return v.substr(0, end);
        return v;
    }

} // namespace undo_cxx
#endif // __ID_SYSTEM_DEFINED

#if !defined(__FACTORY_T_DEFINED)
#define __FACTORY_T_DEFINED
namespace undo_cxx::util::factory {

    /**
     * @brief a factory template class
     * @tparam unique 
     * @tparam product_base 
     * @tparam products 
     * @see https://hedzr.com/c++/algorithm/cxx17-factory-pattern/
     */
    template<typename product_base, typename... products>
    class factory final {
    public:
        CLAZZ_NON_COPYABLE(factory);
        using string = id_type;
        template<typename T>
        struct clz_name_t {
            string id = id_name<T>();
            typedef T type;
            typedef product_base base_type;
            static void static_check() {
                static_assert(std::is_base_of<product_base, T>::value, "all products must inherit from product_base");
            }
            template<typename... Args>
            std::unique_ptr<base_type> gen(Args &&...args) const {
                return std::make_unique<T>(args...);
            }
            T data;
        };
        using named_products = std::tuple<clz_name_t<products>...>;
        // using _T = typename std::conditional<unique, std::unique_ptr<product_base>, std::shared_ptr<product_base>>::type;

        template<typename... Args>
        static auto create(string const &id, Args &&...args) {
            std::unique_ptr<product_base> result{};

            // std::apply([](auto &&...it) {
            //     ((it.static_check()/*static_check<decltype(it.data)>()*/), ...);
            // },
            //            named_products{});

            std::apply([&](auto &&...it) {
                ((it.id == id ? result = (it.static_check(), it.gen(args...)) /*std::make_unique<decltype(it.data)>(args...)*/ : result), ...);
            },
                       named_products{});
            return result;
        }
        template<typename... Args>
        static std::shared_ptr<product_base> make_shared(string const &id, Args &&...args) {
            std::shared_ptr<product_base> ptr = create(id, args...);
            return ptr;
        }
        template<typename... Args>
        static std::unique_ptr<product_base> make_unique(string const &id, Args &&...args) {
            return create(id, args...);
        }
        template<typename... Args>
        static product_base *create_nacked_ptr(string const &id, Args &&...args) {
            return create(id, args...).release();
        }

    private:
        // template<typename product>
        // static void static_check() {
        //     static_assert(std::is_base_of<product_base, product>::value, "all products must inherit from product_base");
        // }
    }; // class factory

} // namespace undo_cxx::util::factory
#endif //__FACTORY_T_DEFINED

#endif //UNDO_CXX_UNDO_UTIL_HH
