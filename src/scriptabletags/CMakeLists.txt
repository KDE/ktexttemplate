# SPDX-FileCopyrightText: Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

add_library(ktexttemplate_scriptabletags MODULE
    scriptablecontext.cpp
    scriptablefilterexpression.cpp
    scriptablenode.cpp
    scriptableparser.cpp
    scriptablesafestring.cpp
    scriptabletags.cpp
    scriptabletemplate.cpp
    scriptablevariable.cpp
    scriptablefilter.cpp
)
set_property(TARGET ktexttemplate_scriptabletags PROPERTY
    EXPORT_NAME scriptabletags
)
target_link_libraries(ktexttemplate_scriptabletags PRIVATE
    KF6::TextTemplate
    Qt6::Qml
)
ktexttemplate_adjust_plugin_name(ktexttemplate_scriptabletags)

install(TARGETS ktexttemplate_scriptabletags
  EXPORT KF6TextTemplateTargets
  DESTINATION ${KTextTemplate_INSTALL_PLUGINDIR}
)
