# ace make file
#
# This file is part of the ACE command line editor.
#
# SPDX-FileCopyrightText: 1980-2026  Andrew C. Starritt
# SPDX-License-Identifier: GPL-3.0-only
#
# Contact details:
# andrew.starritt@gmail.com
#

# Allows make to be run from the top level.
#
.PHONY : all clean  uninstall help FORCE

# Currently only one sub-directory.
#
SUBDIRS = src

all install clean uninstall: $(SUBDIRS)

# Note the all, clean, uninstall targets are passed from command-line
# via $(MAKECMDGOALS), are handled by each sub-directory's Makefile
# targets.

$(SUBDIRS): FORCE
	$(MAKE) -C $@  $(MAKECMDGOALS) 

help:
	@echo "Just type 'make' to build ace. Run 'make install' to build and install"
	@echo "ace to this location:  /usr/local/bin/ace"
	@echo "The use of sudo before 'make install' is not required as the sudo call" 
	@echo "is included within the Makefile itself."
	@echo "Note: There is no configure step."
	@echo ""

# Force targets.
#
FORCE:

# end
