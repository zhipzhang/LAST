

add_library(LASTImage SHARED LCleaning.cpp LImageProcessor.cpp ../Instrument/LCameraGeometry.cpp)
target_include_directories(LASTImage PUBLIC ${ExternalInc} ${COREDIR})
target_link_libraries(LASTImage PUBLIC LASTDL0 LASTDL1)
INSTALL(TARGETS LASTImage DESTINATION ${CMAKE_LIB_DIR})
#target_include_directories(LASTImageProcess PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${COREDIR} ${ExternalInc})