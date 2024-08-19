cmake_minimum_required(VERSION 3.20..)

#### PREPARATIONS ##################################################
set(CMAKE_SCRIPTS "cmake")
set(CMAKE_MODULE_PATH
		${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}/modules
		${CMAKE_SOURCE_DIR}/${CMAKE_SCRIPTS}
		${CMAKE_SOURCE_DIR}  # for .version.cmake & .options.cmake
		${CMAKE_MODULE_PATH})
#### INCLUDES ##################################################
include(prerequisites)
# include(dummy-project)

set(PROJECT_MACRO_NAME undo_cxx)
set(PROJECT_MACRO_SHORT_NAME undo)
set(PROJECT_MACRO_MID_NAME undo-cxx)
set(PROJECT_MACRO_PREFIX "UNDO_CXX")


#### OPTIONS ##################################################

# cmake -S. -Bbuild -DUNDO_CXX_STANDARD:STRING=20
#set(UNDO_CXX_STANDARD 20 CACHE STRING "Define The C++ Standard, default is 17/20")
set(UNDO_CXX_STANDARD 17 CACHE STRING "Define The C++ Standard, default is 17/20")

#option(UNDO_CXX_BUILD_TESTS_EXAMPLES "Build Test Examples OK" OFF)
#option(UNDO_CXX_BUILD_DOCS "Build Docs-OK" OFF)

option(UNDO_CXX_ENABLE_ASSERTIONS "Enable Assertions - OK" ON)
option(UNDO_CXX_ENABLE_PRECONDITION_CHECKS "Enable Precondition Checks - OK" ON)
option(DP_CXX_ENABLE_VERBOSE_LOG "Enable `dbg_verbose_debug` macro definition (TRACE MODE)" OFF)
option(UNDO_CXX_TEST_THREAD_POOL_DBGOUT "Enable `pool_debug` macro definition" OFF)
#option(UNDO_CXX_ENABLE_THREAD_POOL_READY_SIGNAL "Enable ready signal actions in thread_pool template class" ON)
option(UNDO_CXX_UNIT_TEST "Enable the extra unit-tests" OFF)


#### PROJECTS PREPARE ##################################################

include(CTest)

find_package(Threads REQUIRED)


#### PROJECTS ##########################################################

project(${PROJECT_MACRO_NAME}
		VERSION ${VERSION}
		DESCRIPTION "undo-cxx is a C++17/20 library to provide a undo/redo subsystem (linear or restricted non-linear).."
		LANGUAGES CXX)

if (UNDO_CXX_STANDARD)
	set(CMAKE_CXX_STANDARD ${UNDO_CXX_STANDARD})
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_EXTENSIONS OFF)
endif ()

message(STATUS "---- define ${PROJECT_NAME} / ${PROJECT_MACRO_NAME} / C++ ${UNDO_CXX_STANDARD} ------------")
define_cxx_library_project(${PROJECT_NAME}
		INTERFACE
		PREFIX ${PROJECT_MACRO_PREFIX}
		INSTALL GENERATE_CONFIG
		
		#        SOURCES
		#        core.cc
		#        driver.cc
		
		HEADERS
		${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_MACRO_NAME}.hh
		${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-version.hh
		${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.hh
		#${CMAKE_CURRENT_SOURCE_DIR}/include/undo_cxx.hh
		#${CMAKE_GENERATED_DIR}/${PROJECT_NAME}-version.hh
		#${CMAKE_GENERATED_DIR}/${PROJECT_NAME}-config.hh
		${CMAKE_CURRENT_SOURCE_DIR}/include/undo_cxx/undo.hh
		${CMAKE_CURRENT_SOURCE_DIR}/include/undo_cxx/undo-common.hh
		${CMAKE_CURRENT_SOURCE_DIR}/include/undo_cxx/undo-dbg.hh
		${CMAKE_CURRENT_SOURCE_DIR}/include/undo_cxx/undo-def.hh
		${CMAKE_CURRENT_SOURCE_DIR}/include/undo_cxx/undo-log.hh
		${CMAKE_CURRENT_SOURCE_DIR}/include/undo_cxx/undo-util.hh
		${CMAKE_CURRENT_SOURCE_DIR}/include/undo_cxx/undo-zcore.hh
		
		DETAILED_HEADERS
		${CMAKE_CURRENT_SOURCE_DIR}/include/undo_cxx/detail/undo-if.hh
)
message(STATUS "---- defined ${PROJECT_NAME} / ${PROJECT_MACRO_NAME} ------------")


##### TESTS ##################################################
#
#if ((${UNDO_CXX_BUILD_TESTS_EXAMPLES}) OR ("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}"))
#    enable_testing()
#    add_subdirectory(examples/)
#    add_subdirectory(tests/)
#endif ()

message(STATUS "tail of top level :  Using default C++ Standard ${UNDO_CXX_STANDARD}")
