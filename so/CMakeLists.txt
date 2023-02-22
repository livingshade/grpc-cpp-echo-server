file(GLOB SO_SRC ${PROJECT_SOURCE_DIR}/so/*.cpp)

foreach(SO ${SO_SRC})
    get_filename_component(SO_NAME ${SO} NAME_WE)
    add_library(${SO_NAME} SHARED ${SO})
    target_link_libraries(${SO_NAME} PUBLIC project_obj gtest_main)
    add_test(NAME ${SO_NAME} COMMAND ${SO_NAME})
endforeach()

enable_testing()