add_executable(last-convert LASTRawConvert.cpp)
target_link_libraries(last-convert LASTRAW)
target_include_directories(last-convert PRIVATE ${COREDIR})

add_executable(last-dl0gen LASTDL0Gen.cpp)
target_link_libraries(last-dl0gen PRIVATE LASTDL0 LASTRAW)
target_include_directories(last-dl0gen PRIVATE ${COREDIR})


add_executable(last-dl1gen LASTDL1Gen.cpp )
target_link_libraries(last-dl1gen PRIVATE LASTImage)
target_include_directories(last-dl1gen PRIVATE ${COREDIR})

add_executable(last-hillasreco LASTDL1bGen.cpp)
target_link_libraries(last-hillasreco PRIVATE LASTREC)
target_include_directories(last-hillasreco PRIVATE ${COREDIR})

add_executable(last-dl1ana LASTDL1Ana.cpp)
target_link_libraries(last-dl1ana PRIVATE LASTREC)
target_include_directories(last-dl1ana PRIVATE ${COREDIR})

set(MY_EXECUTABLE_TARGETS last-convert last-dl0gen last-dl1gen last-hillasreco last-dl1ana)
INSTALL(TARGETS ${MY_EXECUTABLE_TARGETS}
    RUNTIME DESTINATION ${CMAKE_BIN_DIR})

foreach(TARGET_NAME IN LISTS MY_EXECUTABLE_TARGETS)
    set_target_properties(${TARGET_NAME} PROPERTIES
                          INSTALL_RPATH "$ORIGIN/../lib")
endforeach()

