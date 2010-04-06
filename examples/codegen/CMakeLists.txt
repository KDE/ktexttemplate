project(codegen_grantlee)

cmake_minimum_required(VERSION 2.6)

find_package(Qt4 REQUIRED)
find_package(Grantlee REQUIRED)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_GUI_LIB)
ADD_DEFINITIONS(-DQT_NO_KEYWORDS)

include_directories(
  ${QT_INCLUDES}
  ${QT_QTGUI_INCLUDE_DIR}
  ${QT_QTSQL_INCLUDE_DIR}
  ${Grantlee_INCLUDE_DIR}
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}
)

configure_file(grantlee_paths.h.cmake ${PROJECT_BINARY_DIR}/grantlee_paths.h)

set(codegen_example_SRCS
  comboboxdelegate.cpp
  main.cpp
  methodmodel.cpp
  codegentableview.cpp
  designwidget.cpp
  propertytablewidget.cpp
  mainwindow.cpp
)

qt4_automoc( codegen_example_MOC_SRCS ${codegen_example_SRCS} )

set(codegen_example_RCSS
  codegen.qrc
)

qt4_add_resources(
  codegen_example_RCS_SRCS
  ${codegen_example_RCSS}
)

set(codegen_example_tags_RCSS
  custom_tags.qrc
)

qt4_add_resources(
  codegen_example_RCS_SRCS
  ${codegen_example_tags_RCSS}
  OPTIONS -root "/plugins/grantlee/${Grantlee_VERSION_MAJOR}.${Grantlee_VERSION_MINOR}/"
)

set(codegen_example_UI
  designwidget.ui
)

qt4_wrap_ui(codegen_example_UI_SRCS ${codegen_example_UI})

add_executable(codegen
  ${codegen_example_SRCS}
  ${codegen_example_RCS_SRCS}
  ${codegen_example_UI_SRCS}
)

target_link_libraries(
  codegen
  ${QT_QTGUI_LIBRARIES}
  ${QT_QTSQL_LIBRARIES}
  ${Grantlee_CORE_LIBRARIES}
)