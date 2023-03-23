project(test
        VERSION ${VERSION}
        DESCRIPTION "tests - test apps for cxx17 library"
        LANGUAGES C CXX)

set(PROJECT_ARCHIVE_NAME ${PROJECT_NAME}s-${PROJECT_VERSION})

if (ENABLE_TESTS)
    # include(deps_inc_catch2)
    # include(deps_inc_fmt)
endif ()

define_test_program(detects detects.cc LIBRARIES libs::undo_cxx)
define_test_program(lambda lambda.cc LIBRARIES libs::undo_cxx)
define_test_program(partial-curry partial-curry.cc LIBRARIES libs::undo_cxx)
define_test_program(type-name type-name.cc LIBRARIES libs::undo_cxx)
define_test_program(factory factory.cc LIBRARIES libs::undo_cxx)

define_test_program(basic basic.cc LIBRARIES libs::undo_cxx)

define_test_program(undo-basic undo-basic.cc LIBRARIES libs::undo_cxx)
define_test_program(undo undo.cc LIBRARIES libs::undo_cxx)


message(STATUS "END of tests")