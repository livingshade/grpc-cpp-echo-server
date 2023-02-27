file(GLOB SO_SRC ${PROJECT_SOURCE_DIR}/so/*.cpp)
file(GLOB SO_INCLUDE ${PROJECT_SOURCE_DIR}/include)
message(${SO_SRC} ${SO_INCLUDE})

foreach(SO ${SO_SRC})
    get_filename_component(SO_NAME ${SO} NAME_WE)
    add_library(${SO_NAME} SHARED ${SO})
    target_include_directories(${SO_NAME} PUBLIC ${SO_INCLUDE})
    target_link_libraries(${SO_NAME} PUBLIC grpc_proto ${_REFLECTION} ${_PROTOBUF_LIBPROTOBUF})
endforeach()

enable_testing()