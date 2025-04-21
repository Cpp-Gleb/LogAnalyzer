cmake_minimum_required(VERSION 3.10)
project(AnalyzeLog)

set(CMAKE_CXX_STANDARD 17)

add_executable(AnalyzeLog main.cpp
        LogEntry.cpp
        LogEntry.h
        LogAnalyzer.cpp
        LogAnalyzer.h)
