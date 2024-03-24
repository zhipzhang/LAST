add_library(LASTREC SHARED LHillasReconstructor.cpp)
target_include_directories(LASTREC PUBLIC ${ExternalInc} ${COREDIR})
target_link_libraries(LASTREC PUBLIC LASTDL1)
INSTALL(TARGETS LASTREC DESTINATION ${CMAKE_LIB_DIR})

