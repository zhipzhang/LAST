set(Konrad_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Konrad_hessio)
set(LASTIO_SOURCES EventIO_File.cpp SimTel_File.cpp ${Konrad_DIR}/fileopen.c ${Konrad_DIR}/straux.c)


add_library(LASTIO SHARED ${LASTIO_SOURCES})
target_link_libraries(LASTIO PRIVATE XrdCl XrdXml XrdUtils ${ROOT_LIBRARIES})
target_link_libraries(
    LASTIO PRIVATE spdlog::spdlog_header_only  nlohmann_json::nlohmann_json gflags::gflags)
target_include_directories(LASTIO PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<BUILD_INTERFACE:${Konrad_DIR}> ${ROOT_INCLUDE_DIRS}) 
target_link_libraries(LASTIO PRIVATE ${ZSTD_LIBRARY})
set_target_properties(LASTIO PROPERTIES Version 1.0.0)
INSTALL(TARGETS LASTIO LIBRARY DESTINATION ${CMAKE_LIB_DIR})
set_target_properties(LASTIO PROPERTIES
                          INSTALL_RPATH "$ORIGIN/../lib")
