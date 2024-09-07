#
# check CPU bits
#

project(dummy
	DESCRIPTION "dummy-project for sizeof_void_p detection and more."
	LANGUAGES CXX)

# if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
# # 64 bits
# set(CPU_ARCH "x64" CACHE STRING "ARCH x86_64" FORCE)
# set(CPU_ARCH_NAME "amd64" CACHE STRING "ARCH x86_64" FORCE)
# message(STATUS "x86_64 ...")
# elseif (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
# # 32 bits
# set(CPU_ARCH "x86" CACHE STRING "ARCH x86" FORCE)
# set(CPU_ARCH_NAME "x86" CACHE STRING "ARCH x86_64" FORCE)
# message(STATUS "x86 (32bit) ...")
# else ()
# # message(STATUS "Unknown CMAKE_SIZEOF_VOID_P = '${CMAKE_SIZEOF_VOID_P}' ..?")
# if ("${CMAKE_HOST_SYSTEM_PROCESSOR}" MATCHES "(amd64)|(AMD64)|(x86_64)|(arm64)|(aarch64)")
# set(CPU_ARCH "x64" CACHE STRING "ARCH x86_64" FORCE)
# #set(CPU_ARCH_NAME "x86_64" CACHE STRING "ARCH x86_64" FORCE)
# set(CPU_ARCH_NAME "amd64" CACHE STRING "ARCH x86_64" FORCE)
# message(STATUS "x86_64 ...")
# else ()
# set(CPU_ARCH "x86" CACHE STRING "ARCH x86" FORCE)
# set(CPU_ARCH_NAME "x86" CACHE STRING "ARCH x86_64" FORCE)
# message(STATUS "x86 (32bit) // '${CMAKE_HOST_SYSTEM_PROCESSOR}' ...")
# endif ()
# endif ()

# message(STATUS "Unknown CMAKE_SIZEOF_VOID_P = '${CMAKE_SIZEOF_VOID_P}' ..?")
if(${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "(amd64)|(AMD64)|(x86_64)")
	set(CPU_ARCH "x64" CACHE STRING "ARCH x86_64" FORCE)
	set(CPU_ARCH_BITS "64" CACHE STRING "ARCH x86_64" FORCE)
	set(CPU_ARCH_NAME "amd64" CACHE STRING "ARCH x86_64" FORCE)
	message(STATUS "dummy > arch: x86_64 ...")
elseif(${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "(arm64)|(aarch64)")
	set(CPU_ARCH "x64" CACHE STRING "ARCH aarch64/arm64" FORCE)
	set(CPU_ARCH_BITS "64" CACHE STRING "ARCH aarch64/arm64" FORCE)
	set(CPU_ARCH_NAME "arm64" CACHE STRING "ARCH aarch64/arm64" FORCE)
	message(STATUS "dummy > arch: arm64 ...")
elseif(${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES "(arm7)|(armv8)")
	set(CPU_ARCH "x32" CACHE STRING "ARCH armv7/armv8" FORCE)
	set(CPU_ARCH_BITS "32" CACHE STRING "ARCH armv7/armv8" FORCE)
	set(CPU_ARCH_NAME "arm32" CACHE STRING "ARCH arm32" FORCE)
	message(STATUS "dummy > arch: arm32 ...")
elseif(${CMAKE_HOST_APPLE})
	set(CPU_ARCH_BITS "64" CACHE STRING "ARCH apple x86_64 or arm64" FORCE)

	if(${CMAKE_APPLE_SILICON_PROCESSOR} MATCHES "arm64")
		set(CPU_ARCH "x64" CACHE STRING "ARCH aarch64/arm64" FORCE)
		set(CPU_ARCH_NAME "arm64" CACHE STRING "ARCH aarch64/arm64" FORCE)
		message(STATUS "dummy > arch: arm64 ...")
	else()
		set(CPU_ARCH "x64" CACHE STRING "ARCH x86_64" FORCE)
		set(CPU_ARCH_NAME "amd64" CACHE STRING "ARCH x86_64" FORCE)
		message(STATUS "dummy > arch: x86_64 ...")
	endif()
else()
	set(CPU_ARCH "x86" CACHE STRING "ARCH x86" FORCE)
	set(CPU_ARCH_BITS "32" CACHE STRING "ARCH x86" FORCE)
	set(CPU_ARCH_NAME "x86" CACHE STRING "ARCH x86" FORCE)
	message(STATUS "dummy > arch: x86 (32bit) // '${CMAKE_HOST_SYSTEM_PROCESSOR}' ...")
endif()

#
#
#
include(target-dirs)
include(cxx-detect-compilers) # detect cxx compiler
include(detect-systems) # detect OS, ...
include(cxx-macros) # macros: define_cxx_executable_project, ...
include(version-def) # load .version.cmake
include(versions-gen) # generate config.h and version.hh

# when using compiler with cmake multi-config feature, a special build type 'Asan' can be used for sanitizing test.
enable_sanitizer_for_multi_config()
