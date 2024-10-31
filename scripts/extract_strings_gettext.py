#! /usr/bin/env python
# -*- coding: utf-8 -*-

##
# SPDX-FileCopyrightText: 2010 Stephen Kelly <steveire@gmail.com>
#
# SPDX-License-Identifier: BSD-2-Clause
##


from strings_extractor_gettext import GettextExtractStrings
import os, sys, glob, operator

if __name__ == "__main__":
  ex = GettextExtractStrings()

  outputfile = sys.stdout

  files = reduce(operator.add, map(glob.glob, sys.argv[1:]))

  for filename in files:
    f = open(filename, "r")
    ex.translate(f, outputfile)

  outputfile.write("\n")



