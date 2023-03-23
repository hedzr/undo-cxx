project(examples
        VERSION ${VERSION}
        DESCRIPTION "examples - all examples apps for undo-cxx library"
        LANGUAGES CXX)

set(PROJECT_ARCHIVE_NAME ${PROJECT_NAME}s-${PROJECT_VERSION})

if (ENABLE_TESTS)
    # include(deps_inc_catch2)
    # include(deps_inc_fmt)
endif ()

define_example_program(simple-wp simple-wp.cc LIBRARIES libs::undo_cxx)

message(STATUS "END of tests")