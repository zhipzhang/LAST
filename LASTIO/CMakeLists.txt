set(Konrad_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Konrad_hessio)
set(LASTIO_SOURCES EventIO_File.cpp SimTel_File.cpp ${Konrad_DIR}/fileopen.c ${Konrad_DIR}/straux.c)

find_library(XRDCL_LIBRARY NAMES XrdCl PATHS /data/home/zhipz/opt/root/lib NO_DEFAULT_PATH)

# 检查是否找到了库
if(XRDCL_LIBRARY)
    message(STATUS "Found XrdCl: ${XRDCL_LIBRARY}")
else()
    message(FATAL_ERROR "XrdCl not found")
endif()

add_library(LASTIO SHARED ${LASTIO_SOURCES})
target_link_libraries(LASTIO PUBLIC ${XRDCL_LIBRARY} ${ROOT_LIBRARIES})
target_link_libraries(
    LASTIO PRIVATE spdlog::spdlog_header_only  nlohmann_json::nlohmann_json gflags::gflags)
target_include_directories(LASTIO PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<BUILD_INTERFACE:${Konrad_DIR}> ${ROOT_INCLUDE_DIRS})

set_target_properties(LASTIO PROPERTIES Version 1.0.0)
