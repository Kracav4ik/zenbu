set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/crc")

set(CRC_NAME crc)

set(QT_LIBS
        Qt5::Core
        Qt5::Widgets
)

set(CRC_SOURCES
        crc.cpp
        CrcMainWindow.h
        CrcMainWindow.cpp
)

add_executable(${CRC_NAME} ${CRC_SOURCES})
target_link_libraries(${CRC_NAME} ${QT_LIBS})
