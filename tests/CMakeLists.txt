set(GTEST_DIR $ENV{GTEST_PATH})
add_subdirectory(${GTEST_DIR} googletest)

set(TEST_SOURCES
    oneshot_timer_test.cpp
    periodic_timer_test.cpp
)

foreach(test_source ${TEST_SOURCES})
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source})

    target_include_directories(${test_name}
        PRIVATE
        ${GTEST_DIR}/include
        PRIVATE
        ${PROJECT_SOURCE_DIR}/include
    )
    target_link_libraries(${test_name} gtest gtest_main)
    target_link_libraries(${test_name} Timer)
    
    include(GoogleTest)
    gtest_discover_tests(${test_name})
endforeach()
