project(books_grantlee)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/ )

find_package(Qt4 REQUIRED)
find_package(Automoc4 REQUIRED)
find_package(Grantlee REQUIRED)


add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_GUI_LIB)

include_directories(
  ${QT_INCLUDES}
  ${QT_QTGUI_INCLUDE_DIR}
  ${QT_QTSQL_INCLUDE_DIR}
  ${GRANTLEE_INCLUDE_DIR}
  ${PROJECT_BINARY_DIR}
)

configure_file(grantlee_paths.h.cmake ${PROJECT_BINARY_DIR}/grantlee_paths.h)

set(books_example_SRCS
  main.cpp
  bookwindow.cpp
  bookwrapper.cpp
  bookdelegate.cpp
)

set(books_example_RCSS
  books.qrc
)

set(books_example_UI
  bookwindow.ui
)

QT4_WRAP_UI(books_example_UI_SRCS ${books_example_UI})

qt4_add_resources(
  books_example_RCS_SRCS
  ${books_example_RCSS}
)

automoc4_add_executable(books
  ${books_example_SRCS}
  ${books_example_RCS_SRCS}
  ${books_example_UI_SRCS}
)

target_link_libraries(
  books
  ${QT_QTGUI_LIBRARIES}
  ${QT_QTSQL_LIBRARIES}
  ${GRANTLEE_LIBRARY}
)


