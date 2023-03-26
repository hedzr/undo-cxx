// undo_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/5.
//

#ifndef UNDO_CXX_UNDO_DEF_HH
#define UNDO_CXX_UNDO_DEF_HH

#include "undo_cxx-config.hh"
#include "undo_cxx-version.hh"

#if !defined(DEBUG) && defined(USE_DEBUG) && USE_DEBUG
#define DEBUG 1
#endif
#if !defined(_DEBUG) && defined(DEBUG)
#define _DEBUG DEBUG
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define OS_WIN 1
#endif

#ifndef _UNUSED_DEFINED
#define _UNUSED_DEFINED

#ifdef __clang__

//#ifndef UNUSED
//#define UNUSED(...) [__VA_ARGS__](){}
//#endif

/**
 * @brief UNUSED macro 
 * @tparam Args 
 * @param args 
 * @code{c++}
 *   UNUSED(argc);
 *   UNUSED(argc, argv);
 *   // Cannot be used for variadic parameters:
 *   //   template&lt;class... Args> void unused_func(Args &&...args) { UNUSED(args); }
 *   // But you can expand the parameter pack like this:
 *   //   template&lt;typename... Args>
 *   //   inline void unused_func(Args &&...args) {
 *   //       UNUSED(sizeof...(args));
 *   //   }
 * @endcode
 */
template<typename... Args>
inline void UNUSED([[maybe_unused]] Args &&...args) {
  (void) (sizeof...(args));
}

#elif __GNUC__ || _MSC_VER

// c way unused
#ifndef UNUSED
#define UNUSED0()
#define UNUSED1(a) (void) (a)
#define UNUSED2(a, b) (void) (a), UNUSED1(b)
#define UNUSED3(a, b, c) (void) (a), UNUSED2(b, c)
#define UNUSED4(a, b, c, d) (void) (a), UNUSED3(b, c, d)
#define UNUSED5(a, b, c, d, e) (void) (a), UNUSED4(b, c, d, e)

#define VA_NUM_ARGS_IMPL(_0, _1, _2, _3, _4, _5, N, ...) N
#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(100, ##__VA_ARGS__, 5, 4, 3, 2, 1, 0)

#define ALL_UNUSED_IMPL_(nargs) UNUSED##nargs
#define ALL_UNUSED_IMPL(nargs) ALL_UNUSED_IMPL_(nargs)
#define UNUSED(...)                         \
  ALL_UNUSED_IMPL(VA_NUM_ARGS(__VA_ARGS__)) \
  (__VA_ARGS__)
#endif

#endif
#endif //_UNUSED_DEFINED

//

#ifndef CLAZZ_NON_COPYABLE
#define CLAZZ_NON_COPYABLE(clz)         \
  clz(const clz &) = delete;            \
  clz(clz &&) noexcept = delete;        \
  clz &operator=(const clz &) = delete; \
  clz &operator=(clz &&) noexcept = delete
#endif

#ifndef CLAZZ_NON_MOVEABLE
#define CLAZZ_NON_MOVEABLE(clz)  \
  clz(clz &&) noexcept = delete; \
  clz &operator=(clz &&) noexcept = delete
#endif

////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#if OS_WIN
#include <sstream>
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
// #define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#undef min
#undef max
#include <time.h>
namespace undo_cxx { namespace cross {
    inline void setenv(const char *__name, const char *__value, int __overwrite = 1) {
      (void) (__overwrite);
      std::ostringstream os;
      os << __name << '=' << __value;
      (void) _putenv(os.str().c_str());
    }

    inline time_t time(time_t *_t = nullptr) {
      return ::time(_t);
    }
    // BEWRAE: this is a thread-unsafe routine, it's just for the simple scene.
    inline struct tm *gmtime(time_t const *_t = nullptr) {
      static struct tm _tm {};
      if (!_t) {
        time_t vt = time();
        gmtime_s(&_tm, &vt);
      } else
        gmtime_s(&_tm, _t);
      return &_tm;
    }

    template<class T>
    inline T max(T a, T b) { return a < b ? b : a; }
    template<class T>
    inline T min(T a, T b) { return a < b ? a : b; }
}} // namespace undo_cxx::cross
#else
#include <algorithm>
#include <ctime>
#include <time.h>
namespace undo_cxx { namespace cross {
    inline void setenv(const char *__name, const char *__value, int __overwrite = 1) {
      ::setenv(__name, __value, __overwrite);
    }

    inline time_t time(time_t *_t = nullptr) {
      return std::time(_t);
    }
    inline struct tm *gmtime(time_t const *_t = nullptr) {
      if (!_t) {
        time_t vt = time();
        return std::gmtime(&vt);
      }
      return std::gmtime(_t);
    }

    template<class T>
    inline T max(T a, T b) { return std::max(a, b); }
    template<class T>
    inline T min(T a, T b) { return std::min(a, b); }
}} // namespace undo_cxx::cross
#endif

#endif //UNDO_CXX_UNDO_DEF_HH
