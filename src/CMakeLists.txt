include_directories(.)

add_executable(
  main

  main.cpp
)
target_link_libraries(
  main
  ff
  snark
  gmp
  zm
  gmpxx
)
target_include_directories(
  main

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
)
