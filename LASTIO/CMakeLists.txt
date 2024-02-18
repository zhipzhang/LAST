set(Konrad_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Konrad_hessio)
set(LASTIO_SOURCES EventIO_File.cpp SimTel_File.cpp ${Konrad_DIR}/fileopen.c ${Konrad_DIR}/straux.c)

add_library(LASTIO SHARED ${LASTIO_SOURCES})
target_link_libraries(LASTIO PUBLIC ${XROOTD_CLIENT_LIBRARIES})
target_link_libraries(
    LASTIO PRIVATE glog::glog nlohmann_json::nlohmann_json gflags::gflags)
target_include_directories(LASTIO PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<BUILD_INTERFACE:${Konrad_DIR}> ${XROOTD_INCLUDE_DIRS})

set_target_properties(LASTIO PROPERTIES Version 1.0.0)
