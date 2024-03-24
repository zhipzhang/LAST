set(DL1HEADERFS ${CMAKE_CURRENT_SOURCE_DIR}/LDL1TelEvent.hh ${CMAKE_CURRENT_SOURCE_DIR}/LDL1bTelEvent.hh ${CMAKE_CURRENT_SOURCE_DIR}/LDL1bArrayEvent.hh ${CMAKE_CURRENT_SOURCE_DIR}/LHillasParameters.hh PARENT_SCOPE)
set(DL1SRC LDL1bEvent.cpp LDL1Event.cpp LRDL1bEvent.cpp LRDL1Event.cpp LHillasParameters.cpp )

add_library(LASTDL1 SHARED ${DL1SRC})
target_link_libraries(LASTDL1 PUBLIC BASE)
#target_link_libraries(LASTDL1 PUBLIC  spdlog::spdlog_header_only gflags::gflags nlohmann_json::nlohmann_json  ${ROOT_LIBRARIES} BASE)
target_include_directories(LASTDL1 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}  ${ROOT_INCLUDE_DIRS} ${ExternalInc} )
