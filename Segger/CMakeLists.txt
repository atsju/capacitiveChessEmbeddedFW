set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

add_library(SeggerRTT INTERFACE)
target_sources(SeggerRTT INTERFACE  RTT/SEGGER_RTT.c
                                    RTT/SEGGER_RTT_printf.c)
target_include_directories(SeggerRTT INTERFACE RTT)
