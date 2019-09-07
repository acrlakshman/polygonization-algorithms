INCLUDE(CheckCXXCompilerFlag)

IF(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/googletest")
  MESSAGE(FATAL_ERROR "Missing dependencies. Please clone the project with --recursive."
    "It is possible to recover by calling \"git submodule update --init --recursive\"")
ENDIF()

IF (NOT TARGET gtest)
  # gtest targets cmake version 2.6 which throws warnings for policy CMP0042 on
  # newer cmakes. This sets the default global setting for that policy.
  SET(CMAKE_POLICY_DEFAULT_CMP0042 NEW)
  IF(WIN32)
    SET(gtest_force_shared_crt ON
      CACHE INTERNAL "Required so that the libs Runtime is not set to MT DLL")
  ENDIF()

  SET(GOOGLETEST_VERSION 1.8.1) #DUMMY to suppress cmake bug
  ADD_SUBDIRECTORY(googletest/googletest EXCLUDE_FROM_ALL)
  SET_TARGET_PROPERTIES(gtest gtest_main
    PROPERTIES
    FOLDER "ExternalProjectTargets/gtest")

  # Hide gtest project variables
  MARK_AS_ADVANCED(
    BUILD_SHARED_LIBS
    gtest_build_samples
    gtest_build_tests
    gtest_disable_pthreads
    gtest_force_shared_crt
    gtest_hide_internal_symbols)
ENDIF()
