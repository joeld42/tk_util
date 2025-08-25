project(tk_util)
cmake_minimum_required(VERSION 3.5)

include_directories( "." )
include_directories( "c:/Toolkits/stb/" )



add_executable( tkutil_unittest
                    tk_array.h
                    tk_alloc.h
                    tk_alloc.cpp
                    tkutil_unittest.cpp
                    tk_slotlist.h)


add_executable( kmeans_palette
                    tk_array.h
                    tk_alloc.h
                    tools/kmeans_palette.cpp
                    tk_alloc.cpp )