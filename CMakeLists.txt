cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(QtClient VERSION 0.0.1 DESCRIPTION "QtClient" LANGUAGES C CXX)

set(TARGET_NAME "QtClient")
set(DEFAULT_BUILD_TYPE "Debug")
set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS_COMMON}")

set(CMAKE_AUTOMOC ON)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

configure_file(client.json ${CMAKE_BINARY_DIR}/client.json)
configure_file(client.json ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bin/client.json)
configure_file(client.json ${CMAKE_BINARY_DIR}/bin/client.json)
configure_file(client.json ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/client.json)


if (WIN32)
	list(APPEND CMAKE_PREFIX_PATH "E:\\repo\\opencvBuild")
	list(APPEND CMAKE_PREFIX_PATH "E:\\Qt\\5.12.8\\msvc2017_64")
	list(APPEND CMAKE_PREFIX_PATH "E:\\repo\\spdlog\\build\\install")
	list(APPEND CMAKE_PREFIX_PATH "E:\\repo\\IncludeSpdlog\\build\\install")
	list(APPEND CMAKE_PREFIX_PATH "E:\\repo\\ConfigReader\\build\\install")
endif (WIN32)

find_package(spdlog REQUIRED)
find_package(IncludeSpdlog REQUIRED)
find_package(ConfigReader REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Threads REQUIRED)


if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified")
    set(CMAKE_BUILD_TYPE ${DEFAULT_BUILD_TYPE} CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(SRC_MAIN src/main.cc)

set(SRC
	src/mqtsockio.cc
	src/mqt.cc
	src/mqtmessage.cc
    src/broadcaster.cc)

set(HEADERS
	include/mqtsockio.h
	include/mqt.h
	include/mqtmessage.h
    include/broadcaster.h)

add_executable( 
 ${PROJECT_NAME}_app client.json ${SRC} ${SRC_MAIN} ${HEADERS} )

add_library(${PROJECT_NAME} STATIC
    client.json
    ${SRC}
    ${HEADERS}
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src)

target_link_libraries(${PROJECT_NAME} ConfigReader)
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Network Threads::Threads)

target_link_libraries(${PROJECT_NAME}_app ConfigReader)
target_link_libraries(${PROJECT_NAME}_app Qt5::Core Qt5::Network Threads::Threads)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "default install path" FORCE )
endif()

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Config
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES  ${HEADERS} DESTINATION include)
install(EXPORT ${PROJECT_NAME}Config DESTINATION share/${PROJECT_NAME}/cmake)

export(TARGETS ${PROJECT_NAME} FILE ${PROJECT_NAME}Config.cmake)


if (WIN32)
    get_target_property(QT5_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(QT5_WINDEPLOYQT_EXECUTABLE ${QT5_QMAKE_EXECUTABLE} PATH)
    set(QT5_WINDEPLOYQT_EXECUTABLE "${QT5_WINDEPLOYQT_EXECUTABLE}/windeployqt.exe")

    add_custom_command(TARGET ${PROJECT_NAME}_app POST_BUILD
       COMMAND ${QT5_WINDEPLOYQT_EXECUTABLE} --qmldir ${CMAKE_SOURCE_DIR} $<TARGET_FILE_DIR:${PROJECT_NAME}_app>)
endif(WIN32)

if (WIN32)
	macro(qt5_copy_dll APP DLL)
		# find the release *.dll file
		get_target_property(Qt5_${DLL}Location Qt5::${DLL} LOCATION)
		# find the debug *d.dll file
		get_target_property(Qt5_${DLL}LocationDebug Qt5::${DLL} IMPORTED_LOCATION_DEBUG)
		add_custom_command(TARGET ${APP} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:Debug>:${Qt5_${DLL}LocationDebug}> $<$<NOT:$<CONFIG:Debug>>:${Qt5_${DLL}Location}> $<TARGET_FILE_DIR:${APP}>)
	endmacro()
	qt5_copy_dll( ${PROJECT_NAME}_app Core)

endif(WIN32)
