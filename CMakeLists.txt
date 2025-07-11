cmake_minimum_required(VERSION 3.16)
project(IridiumProject)

enable_language(C)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


if(DEFINED DEBUG AND DEBUG STREQUAL "ON")
    add_definitions(-DDEBUG)
endif()

if(DEFINED IMT_QUEUE_SIZE)
    add_definitions(-DIMT_QUEUE_SIZE=${IMT_QUEUE_SIZE})
endif()

if(DEFINED IMT_PAYLOAD_SIZE)
    add_definitions(-DIMT_PAYLOAD_SIZE=${IMT_PAYLOAD_SIZE})
endif()

# Check for libgpiod
find_library(LIB_GPIOD gpiod)

if (NOT LIB_GPIOD)
    message(STATUS "GPIO support is DISABLED (libgpiod not found)")
    set(GPIO_ENABLED FALSE CACHE BOOL "Set if libgpiod is available")
else()
    message(STATUS "GPIO support is ENABLED (libgpiod found)")
    set(GPIO_ENABLED TRUE CACHE BOOL "Set if libgpiod is available")
    add_compile_definitions(RB_GPIO)
endif()

if(WIN32)
    message(STATUS "Building on Windows")
    set(FW_UPDATE_DEFAULT ON)
    add_compile_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(APPLE)
    message(STATUS "Building on macOS")
    set(FW_UPDATE_DEFAULT ON)
elseif(UNIX AND NOT APPLE)
    message(STATUS "Building on Linux")
    set(FW_UPDATE_DEFAULT ON)
else()
    set(FW_UPDATE_DEFAULT OFF)
endif()

option(FW_UPDATE "Enable Kermit firmware update" ${FW_UPDATE_DEFAULT})

# Source directories
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(EXAMPLE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/examples)
set(THIRD_PARTY_DIR ${SRC_DIR}/third_party)
set(KERMIT_DIR ${THIRD_PARTY_DIR}/ekermit)
set(KERMIT_IO_DIR ${SRC_DIR}/kermit)
set(WINDOWS_GET_OPT_DIR ${THIRD_PARTY_DIR}/wingetopt/src)
# Library build here
set(KERMIT_LIB kermit)
set(WINDOWS_GET_OPT_LIB wingetopt)
set(IRIDIUM_IMT_LIB iridiumImt)

# Some parent variables so projects can link against us and include us
get_directory_property(HAS_PARENT PARENT_DIRECTORY)
if (HAS_PARENT)
    set(RB9603_INCLUDES ${SRC_DIR} PARENT_SCOPE)
    set(RB9603_LIB ${IRIDIUM_IMT_LIB} PARENT_SCOPE)
endif()


# Example binaries

include_directories(${SRC_DIR}
    ${KERMIT_DIR}
    ${KERMIT_IO_DIR})

add_subdirectory(${SRC_DIR})

if (GPIO_ENABLED)
    set(GPIO_SRC gpio.c)
else()
    set(GPIO_SRC "")
endif()



# Doxygen documentation generation
find_package(Doxygen)

if(DOXYGEN_FOUND)
    set(DOXYGEN_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/rockblock-9603-doxygen.config")

    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIG_FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
else()
    message(STATUS "Doxygen not found. 'make docs' target will not be available.")
endif()