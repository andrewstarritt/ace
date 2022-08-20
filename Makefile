# ace make file
#
# This file is part of the ACE command line editor.
#
# Copyright (C) 1980-2022  Andrew C. Starritt
#
# The ACE program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by the
# Free Software Foundation, either version 3 of the License, or (at your
# option) any later version.
#
# The ACE program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License along
# with the ACE program. If not, see <http://www.gnu.org/licenses/>.
#
# Contact details:
# andrew.starritt@gmail.com
# PO Box 3118, Prahran East, Victoria 3181, Australia.
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
