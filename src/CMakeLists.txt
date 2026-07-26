set(SOURCES
    main.cpp
    mainwindow.cpp
    gateitem.cpp
)

set(HEADERS
    mainwindow.h
    gateitem.h
    logicengine.h
)

add_executable(jhatkaa
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(jhatkaa PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(jhatkaa PRIVATE ${QT_WIDGETS_TARGET})
