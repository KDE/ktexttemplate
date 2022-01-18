add_library(ktexttemplate_defaulttags MODULE
  defaulttags.cpp
  autoescape.cpp
  comment.cpp
  cycle.cpp
  debug.cpp
  filtertag.cpp
  firstof.cpp
  for.cpp
  if.cpp
  ifchanged.cpp
  ifequal.cpp
  load.cpp
  mediafinder.cpp
  now.cpp
  range.cpp
  regroup.cpp
  spaceless.cpp
  templatetag.cpp
  widthratio.cpp
  with.cpp
)
set_property(TARGET ktexttemplate_defaulttags PROPERTY
  EXPORT_NAME defaulttags
)
target_link_libraries(ktexttemplate_defaulttags PRIVATE
  KF6::TextTemplate
)
target_compile_features(ktexttemplate_defaulttags PRIVATE
  cxx_auto_type
  cxx_variadic_templates
)
ktexttemplate_adjust_plugin_name(ktexttemplate_defaulttags)

install(TARGETS ktexttemplate_defaulttags 
  EXPORT grantlee_targets
  LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR}
  COMPONENT Templates
)
