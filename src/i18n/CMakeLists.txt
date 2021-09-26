add_library(ktexttemplate_i18ntags MODULE
  i18ntags.cpp
  i18n.cpp
  i18nc.cpp
  i18np.cpp
  i18ncp.cpp
  l10n_money.cpp
  l10n_filesize.cpp
  with_locale.cpp
)
set_property(TARGET ktexttemplate_i18ntags PROPERTY
  EXPORT_NAME i18ntags
)
target_link_libraries(ktexttemplate_i18ntags PRIVATE
  KF6::TextTemplate
)
ktexttemplate_adjust_plugin_name(ktexttemplate_i18ntags)

install(TARGETS ktexttemplate_i18ntags
  EXPORT ktexttemplate_targets
  LIBRARY DESTINATION ${PLUGIN_INSTALL_DIR}
  COMPONENT Templates
)
