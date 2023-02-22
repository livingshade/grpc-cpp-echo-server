cmake_minimum_required(VERSION 3.12.4)

project(grpc-cpp-echo-server)

option(BUILD_BINARIES "Build the binaries in {project}/bin directory" ON)
option(BUILD_UNITTEST "Build the unittest in {project}/test directory" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
find_package(glog REQUIRED)
find_package(gflags REQUIRED)

# ########### grpc cmake
# This branch assumes that gRPC and all its dependencies are already installed
# on this system, so they can be located by find_package().

# Find Protobuf installation
# Looks for protobuf-config.cmake file installed by Protobuf's cmake installation.
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)

if(CMAKE_CROSSCOMPILING)
    find_program(_PROTOBUF_PROTOC protoc)
else()
    set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
endif()

# Find gRPC installation
# Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP gRPC::grpc++)

if(CMAKE_CROSSCOMPILING)
    find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
endif()

# ######## grpc end

# protobuf headers
# Proto file
get_filename_component(proto "./pb/echo.proto" ABSOLUTE)
get_filename_component(proto_path "${proto}" PATH)

# Generated sources
set(proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/echo.pb.cc")
set(proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/echo.pb.h")
set(grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/echo.grpc.pb.cc")
set(grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/echo.grpc.pb.h")
add_custom_command(
    OUTPUT "${proto_srcs}" "${proto_hdrs}" "${grpc_srcs}" "${grpc_hdrs}"
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
    -I "${proto_path}"
    --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
    "${proto}"
    DEPENDS "${proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_library(grpc_proto
    ${grpc_srcs}
    ${grpc_hdrs}
    ${proto_srcs}
    ${proto_hdrs})
target_link_libraries(grpc_proto
    PUBLIC
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})

# protobuf end

# dir to include
set(PROJECT_INCLUDE_DIRECTORIES include)

add_compile_options("-Wall" "-W" "-Wextra" "-fPIC")

add_subdirectory(gtest)

# headers to install
file(GLOB_RECURSE PROJECT_PUBLIC_HEADER include/*)

# source for the lib
file(GLOB_RECURSE PROJECT_SOURCE src/*)
file(GLOB_RECURSE SO_SOURCE so/*)

add_library(project_obj OBJECT ${PROJECT_SOURCE})

target_include_directories(project_obj PUBLIC ${PROJECT_INCLUDE_DIRECTORIES})

target_link_libraries(project_obj PUBLIC grpc_proto ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})

target_link_libraries(project_obj PUBLIC ${GLOG_LIBRARIES} ${GFLAGS_LIBRARIES})

target_include_directories(project_obj PUBLIC ${GLOG_INCLUDE_DIRS} ${GFLAGS_INCLUDE_DIRS})

add_subdirectory(so)

if(${BUILD_UNITTEST})
    add_subdirectory(test)
    enable_testing()
endif()

if(${BUILD_BINARIES})
    add_subdirectory(bin)
endif()
