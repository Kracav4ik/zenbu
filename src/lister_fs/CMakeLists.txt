set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/lister_fs")

set(LISTER_FS_NAME lister_fs)

set(QT_LIBS
        Qt5::Core
        Qt5::Widgets
        Qt5::WebSockets
)

set(LISTER_FS_SOURCES
        listerFs.cpp
        ListerFsMainWindow.h
        ListerFsMainWindow.cpp
        ../utils.h
)

add_executable(${LISTER_FS_NAME} ${LISTER_FS_SOURCES})
target_link_libraries(${LISTER_FS_NAME} ${QT_LIBS})
