cmake_minimum_required(VERSION 3.0)

project(physfs_streambuf)

add_library(physfs_streambuf INTERFACE "physfs_streambuf.hpp")
target_include_directories(physfs_streambuf INTERFACE ".")
target_compile_features(physfs_streambuf INTERFACE cxx_std_11)
