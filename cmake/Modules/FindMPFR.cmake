# FindMPFR.cmake

find_path(MPFR_INCLUDE_DIR
  NAMES mpfr.h
  PATH_SUFFIXES mpfr
)

find_library(MPFR_LIBRARY
  NAMES mpfr
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MPFR DEFAULT_MSG MPFR_LIBRARY MPFR_INCLUDE_DIR)

mark_as_advanced(MPFR_INCLUDE_DIR MPFR_LIBRARY)

if(MPFR_FOUND)
  set(MPFR_LIBRARIES ${MPFR_LIBRARY})
  set(MPFR_INCLUDE_DIRS ${MPFR_INCLUDE_DIR})
endif()

