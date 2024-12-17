// undo_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/13.
//

#include "undo_cxx/undo-common.hh"

#include <cmath>
#include <iostream>

namespace {

  static int test(int x, int y, int z) {
    return x + y + z;
  }

  static void test_partial() {
    using namespace undo_cxx::util::cool;
    std::cout << '\n';

    auto f = partial(test, 5, 3);
    std::cout << f(7) << '\n';

    std::cout << partial(test)(5, 3, 7) << '\n';
    std::cout << partial(test, 5)(3, 7) << '\n';
    std::cout << partial(test, 5, 3)(7) << '\n';
  }

  static void test_curry() {
    using namespace undo_cxx::util::cool;
    std::cout << '\n';

    auto f = curry(test)(1);
    auto g = f(2);
    auto result = g(3);
    auto result1 = curry(test)(1)(2)(3);
    std::cout << "result:  " << result << '\n';
    std::cout << "result1: " << result1 << '\n';
  }

} // namespace

int main() {
  test_partial();
  test_curry();
  return 0;
}