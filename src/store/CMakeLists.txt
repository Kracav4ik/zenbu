message("${CMAKE_SOURCE_DIR}/bin/store")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/store")

set(STORE_NAME store)

set(QT_LIBS
        Qt5::Core
        Qt5::Widgets
)

set(STORE_SOURCES
        store.cpp
        StoreMainWindow.cpp
        StoreMainWindow.h
)

add_executable(${STORE_NAME} ${STORE_SOURCES})
target_link_libraries(${STORE_NAME} ${QT_LIBS})
