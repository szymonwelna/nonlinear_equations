cmake_minimum_required(VERSION 3.20.0)
project(rownania_nieliniowe)

# Ustawienie ścieżki do modułów CMake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Znalezienie pakietu MPFR
find_package(MPFR REQUIRED)
if(MPFR_FOUND)
    message(STATUS "Found MPFR: ${MPFR_LIBRARIES}")
else()
    message(FATAL_ERROR "MPFR not found")
endif()

# Znalezienie pakietu Boost
find_package(Boost REQUIRED)
if(Boost_FOUND)
    message(STATUS "Found Boost: ${Boost_LIBRARIES}")
else()
    message(FATAL_ERROR "Boost not found")
endif()

# Dodanie katalogu z plikami nagłówkowymi
include_directories(${MPFR_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)

# Dodanie plików źródłowych projektu
add_executable(MyExecutable main.cpp)

# Linkowanie bibliotek
target_link_libraries(MyExecutable ${MPFR_LIBRARIES} ${Boost_LIBRARIES})
