add_executable(visualiser src/main.cpp)

# Boost
find_package(Boost 1.72.0 COMPONENTS program_options REQUIRED)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(visualiser PRIVATE ${Boost_LIBRARIES})

# Exectuable and linked library.
target_link_libraries(visualiser PRIVATE libvisualiser)

set_target_properties(visualiser PROPERTIES
    CXX_STANDARD          17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS        NO
    )
