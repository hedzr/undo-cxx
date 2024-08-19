//
// Created by Hedzr Yeh on 2021/9/26.
//

#include "undo_cxx.hh"
#include <iomanip>
#include <iostream>

int main() {
  std::cout << "UNDO_CXX_ENABLE_ASSERTIONS               : " << UNDO_CXX_ENABLE_ASSERTIONS << '\n';
  std::cout << "UNDO_CXX_ENABLE_PRECONDITION_CHECKS      : " << UNDO_CXX_ENABLE_PRECONDITION_CHECKS << '\n';
  std::cout << "UNDO_CXX_ENABLE_THREAD_POOL_READY_SIGNAL : " << UNDO_CXX_ENABLE_THREAD_POOL_READY_SIGNAL << '\n';
  std::cout << "UNDO_CXX_TEST_THREAD_POOL_DBGOUT         : " << UNDO_CXX_TEST_THREAD_POOL_DBGOUT << '\n';
  std::cout << "UNDO_CXX_UNIT_TEST                       : " << UNDO_CXX_UNIT_TEST << '\n';

  std::cout << '\n'
            << UNDO_CXX_PROJECT_NAME << " v" << UNDO_CXX_VERSION_STRING << '\n'
            << UNDO_CXX_ARCHIVE_NAME << '\n'
            << "             version: " << UNDO_CXX_VERSION_STR << '\n'
            << "              branch: " << UNDO_CXX_GIT_BRANCH << '\n'
            << "                hash: " << UNDO_CXX_GIT_COMMIT_HASH << '\n'
            << "                 cpu: " << UNDO_CXX_CPU << '\n'
            << "                arch: " << UNDO_CXX_CPU_ARCH << '\n'
            << "           arch-name: " << UNDO_CXX_CPU_ARCH_NAME << '\n'
            << "          build-name: " << UNDO_CXX_BUILD_NAME << '\n'
            << " built C++ Standards: " << __cplusplus << '\n';
}