set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

add_library(BSP INTERFACE) # INETERFACE is because I have no sources (yet ?)
target_include_directories(BSP INTERFACE .)
