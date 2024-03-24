set(DL0HEADERFS  ${CMAKE_CURRENT_SOURCE_DIR}/LDL0TelEvent.hh PARENT_SCOPE)
set(DL0SRC ${TELRAWSRCS} LDL0Event.cpp LDL0TelEvent.cpp LRDL0Event.cpp )
add_library(LASTDL0 STATIC)
target_sources(LASTDL0 PRIVATE ${DL0SRC})
target_link_libraries(LASTDL0 PUBLIC BASE)
#target_link_libraries(LASTDL0 PUBLIC  spdlog::spdlog_header_only gflags::gflags nlohmann_json::nlohmann_json  ${ROOT_LIBRARIES} BASE)
target_include_directories(LASTDL0 PUBLIC ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

