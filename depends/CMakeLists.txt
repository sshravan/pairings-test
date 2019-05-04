add_subdirectory(libsnark)

include_directories(ate-pairing/include)
include_directories(ate-pairing/test/)
include_directories(xbyak)
# link_directories(ate-pairing/lib/)

add_library(
  zm
  STATIC

  ate-pairing/src/zm.cpp
  ate-pairing/src/zm2.cpp
)
