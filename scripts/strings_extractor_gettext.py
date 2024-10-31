#! /usr/bin/env python
# -*- coding: utf-8 -*-

##
# SPDX-FileCopyrightText: 2010 Stephen Kelly <steveire@gmail.com>
#
# SPDX-License-Identifier: BSD-2-Clause
##

from strings_extractor import TranslationOutputter

class GettextExtractStrings(TranslationOutputter):
  def createOutput(self, template_filename, context_strings, outputfile):
    for context_string in context_strings:
      outputfile.write("// i18n: file: " + template_filename + "\n")
      if context_string.context:
        if not context_string.plural:
          outputfile.write("pgettext(\"" + context_string.context + "\", \"" + context_string._string + "\");\n")
        else:
          outputfile.write("npgettext(\"" + context_string.context + "\", \"" + context_string._string + "\", \"" + context_string.plural + "\");\n")
      else:
        if context_string.plural:
          outputfile.write("ngettext(\"" + context_string._string + "\", \"" + context_string.plural + "\");\n")
        else:
          outputfile.write("gettext(\"" + context_string._string + "\");\n")


