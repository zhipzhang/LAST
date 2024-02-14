set(LINKDIR ${CMAKE_CURRENT_SOURCE_DIR}/LinkDefDir)
set(HEADERFS LShower.hh LASTRAW/LTelElectronic.hh  LTelTrueImage.hh LSimulationConfig.hh LCameraSet.hh LTelConfig.hh  LASTDL0/LDL0TelEvent.hh LASTDL1/LDL1TelEvent.hh ${CMAKE_SOURCE_DIR}/LASTCore/Image/LHillasParameters.hh)
set(DLSources LASTRAW/LEventRaw.cpp LASTRAW/LREventRaw.cpp LASTRAW/LEventElectronic.cpp LDataBase.cpp LJsonConfig.cpp
	LTelConfig.cpp LSimulationImage.cpp LCameraSet.cpp LASTDL0/LDL0Event.cpp LASTDL0/LRDL0Event.cpp LASTDL0/LDL0TelEvent.cpp LASTRAW/LTelElectronic.cpp LTelTrueImage.cpp LASTDL1/LDL1Event.cpp LASTDL1/LRDL1Event.cpp LASTConverter/LImageProcessor.cpp ${CMAKE_SOURCE_DIR}/LASTCore/Image/LHillasParameters.cpp ${CMAKE_SOURCE_DIR}/LASTCore/Image/LCleaning.cpp ${CMAKE_SOURCE_DIR}/LASTCore/Instrument/LCameraGeometry.cpp)
add_library(DLs STATIC ${DLSources})
target_link_libraries(DLs PUBLIC LASTIO)
target_link_libraries(DLs PRIVATE glog::glog gflags::gflags nlohmann_json::nlohmann_json  ${ROOT_LIBRARIES})
target_include_directories(DLs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/LASTRAW ${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/LASTDL0 ${CMAKE_SOURCE_DIR}/LASTCore ${CMAKE_SOURCE_DIR}/ExternalLib)
ROOT_GENERATE_DICTIONARY(LDLs_dict ${HEADERFS} MODULE DLs LINKDEF ${LINKDIR}/LDLsLinkDef.h)
