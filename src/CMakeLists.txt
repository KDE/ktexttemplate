
add_subdirectory(lib)

add_subdirectory(loadertags)
add_subdirectory(defaulttags)

add_subdirectory(defaultfilters)

add_subdirectory(i18n)

if(Qt6Qml_FOUND)
    add_subdirectory(scriptabletags)
endif()

