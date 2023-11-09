cmake_minimum_required(VERSION 3.26)
project(University)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(Task21_CoprimeNumbers)
include_directories(Task21_MatrixElementsSearch)
include_directories(Task21_PalindromCheck)
include_directories(Task21_Phonebook)
include_directories(Task21_ThreeLinesIntersection)

add_executable(University
        Task21_Phonebook/StringExtensions.cpp
        Task21_Phonebook/StringExtensions.h
        Task21_Phonebook/Task21_Phonebook.cpp)
