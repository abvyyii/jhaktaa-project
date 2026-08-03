set(SOURCES
    main.cpp
    mainwindow.cpp
    database.cpp
    loginwidget.cpp
    registerwidget.cpp
    dashboardwidget.cpp
    gateitem.cpp
)

set(HEADERS
    mainwindow.h
    database.h
    loginwidget.h
    registerwidget.h
    dashboardwidget.h
    gateitem.h
    logicengine.h
)

add_executable(jhatkaa
    ${SOURCES}
    ${HEADERS}
    ../resources/resources.qrc
)

target_include_directories(jhatkaa PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(jhatkaa PRIVATE ${QT_WIDGETS_TARGET} ${QT_SQL_TARGET})
