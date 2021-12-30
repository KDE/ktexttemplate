
add_subdirectory(lib)

add_subdirectory(loadertags)
add_subdirectory(defaulttags)

add_subdirectory(defaultfilters)

add_subdirectory(i18n)

if (BUILD_TESTING)
  add_subdirectory(tests)
endif()
