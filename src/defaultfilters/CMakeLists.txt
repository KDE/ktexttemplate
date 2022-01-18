add_library(ktexttemplate_defaultfilters MODULE
  defaultfilters.cpp
  datetime.cpp
  integers.cpp
  lists.cpp
  logic.cpp
  misc.cpp
  stringfilters.cpp
)
set_property(TARGET ktexttemplate_defaultfilters PROPERTY
    EXPORT_NAME defaultfilters
)
target_link_libraries(ktexttemplate_defaultfilters PRIVATE
  KF6::TextTemplate
)
target_compile_features(ktexttemplate_defaultfilters PRIVATE
  cxx_auto_type
)
ktexttemplate_adjust_plugin_name(ktexttemplate_defaultfilters)

install(TARGETS ktexttemplate_defaultfilters
  EXPORT grantlee_targets
  LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR}
  COMPONENT Templates
)
