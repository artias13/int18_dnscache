cmake_minimum_required(VERSION 3.12)
project(DNSCache)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(dnscache dnscache/dnscache.cpp)
