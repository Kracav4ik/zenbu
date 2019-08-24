set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/crc")

set(CRC_NAME crc)

set(QT_LIBS
        Qt5::Core
        Qt5::Widgets
        Qt5::WebSockets
)

set(CRC_SOURCES
        crc.cpp
        CrcMainWindow.h
        CrcMainWindow.cpp
        ../utils.h
)

add_executable(${CRC_NAME} ${CRC_SOURCES})
target_link_libraries(${CRC_NAME} ${QT_LIBS})
