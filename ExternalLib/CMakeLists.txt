set(ExternalInc ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
include_directories(${CMAKE_BINARY_DIR}/ExternalLib/xrootd/src)
add_subdirectory(xrootd EXCLUDE_FROM_ALL)
# 安装XrdCl库
install(TARGETS XrdCl  XrdXml XrdUtils
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
add_subdirectory(gflags)
add_subdirectory(spdlog)
add_subdirectory(json)

