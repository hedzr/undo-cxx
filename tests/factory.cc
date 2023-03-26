// undo_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/17.
//

#include "undo_cxx.hh"

#include <iomanip>
#include <iostream>
#include <math.h>
#include <string>

#include <functional>
#include <memory>
#include <random>

#include <deque>
#include <list>
#include <optional>
#include <queue>
#include <stack>
#include <vector>

///////////////////////////////////////////////////////////////////

void test_9() {
  using namespace undo_cxx;
  using namespace std::string_view_literals;
  namespace fct = undo_cxx::util::factory;

  struct Base {
    virtual ~Base() {}
    virtual void run() = 0;
  };
  struct A : public Base {
    ~A() {}
    void run() override { std::cout << 'A' << '\n'; }
  };
  struct B : public Base {
    ~B() {}
    void run() override { std::cout << 'B' << '\n'; }
  };
  struct C : public Base {
    ~C() {}
    void run() override { std::cout << 'C' << '\n'; }
  };

  using Factory = fct::factory<Base, A, B, C>;

  std::cout << "id_name of 'A' = " << '"' << id_name<A>() << '"' << '\n';
  auto p = Factory::create(id_name<A>());
  p->run();
}

///////////////////////////////////////////////////////////////////

// see it at:
// https://godbolt.org/z/G8jE447b4
//

int main() {
  // test_1();
  test_9();
}