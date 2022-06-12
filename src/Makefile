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

.PHONY: all install clean uninstall always

TOP=..
OBJ_DIR  = $(TOP)/obj
BIN_DIR  = $(TOP)/bin

OPTIONS += -Wall -pipe -std=gnu++11

RESOPTS += --input binary
RESOPTS += --output elf64-x86-64
RESOPTS += --binary-architecture i386

OBJECTS  = $(OBJ_DIR)/build_datetime.o
OBJECTS += $(OBJ_DIR)/ace_main.o
OBJECTS += $(OBJ_DIR)/commands.o
OBJECTS += $(OBJ_DIR)/command_parser.o
OBJECTS += $(OBJ_DIR)/data_buffer.o
OBJECTS += $(OBJ_DIR)/global.o

OBJECTS += $(OBJ_DIR)/copyright_info.o
OBJECTS += $(OBJ_DIR)/help_ecce.o
OBJECTS += $(OBJ_DIR)/help_general.o
OBJECTS += $(OBJ_DIR)/help_info.o
OBJECTS += $(OBJ_DIR)/help_syntax.o
OBJECTS += $(OBJ_DIR)/help_usage.o
OBJECTS += $(OBJ_DIR)/license.o
OBJECTS += $(OBJ_DIR)/warranty.o

LINKER += -lreadline
LINKER += -lncurses

SENTINAL = $(OBJ_DIR)/.sentinal
TARGET   = $(BIN_DIR)/ace
INSTALL = /usr/local/bin/ace

all : $(TARGET)  Makefile

install : $(INSTALL)  Makefile

$(INSTALL) : $(TARGET)  Makefile
	@echo "Installing $(TARGET) to $(INSTALL)"  && \
	sudo cp -f $(TARGET) $(INSTALL)
	@echo ""

$(TARGET): $(OBJECTS)  Makefile
	@echo ""
	@mkdir -p $(BIN_DIR)
	g++ $(OPTIONS) $(LINKER)  -o $(TARGET) $(OBJECTS)
	@echo ""


build_datetime.cpp: always
	@echo "updating build_datetime.cpp"
	@echo '#include "build_datetime.h"'  > build_datetime.cpp
	@date -u '+std::string build_datetime () { return "%a %d %b %Y %H:%M:%S %Z"; }'  >> build_datetime.cpp

$(OBJ_DIR)/build_datetime.o : $(SENTINAL) build_datetime.cpp build_datetime.h Makefile
	g++ $(OPTIONS) -o $(OBJ_DIR)/build_datetime.o -c build_datetime.cpp

$(OBJ_DIR)/ace_main.o : $(SENTINAL) ace_main.cpp build_datetime.h data_buffer.h  commands.h command_parser.h  global.h  Makefile
	g++ $(OPTIONS) -o $(OBJ_DIR)/ace_main.o       -c ace_main.cpp

$(OBJ_DIR)/commands.o : $(SENTINAL) commands.cpp commands.h  data_buffer.h  global.h  Makefile
	g++ $(OPTIONS) -o $(OBJ_DIR)/commands.o       -c commands.cpp 

$(OBJ_DIR)/command_parser.o : $(SENTINAL) command_parser.cpp command_parser.h  commands.h  global.h  Makefile
	g++ $(OPTIONS) -o $(OBJ_DIR)/command_parser.o -c command_parser.cpp

$(OBJ_DIR)/data_buffer.o : $(SENTINAL) data_buffer.cpp data_buffer.h  global.h  Makefile
	g++ $(OPTIONS) -o $(OBJ_DIR)/data_buffer.o    -c data_buffer.cpp

$(OBJ_DIR)/global.o : $(SENTINAL) global.cpp  global.h  Makefile
	g++ $(OPTIONS) -o $(OBJ_DIR)/global.o         -c global.cpp

# Resource files
#
$(OBJ_DIR)/copyright_info.o : $(SENTINAL)  copyright_info.txt  Makefile
	objcopy  $(RESOPTS) copyright_info.txt $(OBJ_DIR)/copyright_info.o

$(OBJ_DIR)/help_general.o : $(SENTINAL)  help_general.txt  Makefile
	objcopy  $(RESOPTS) help_general.txt   $(OBJ_DIR)/help_general.o

$(OBJ_DIR)/help_info.o : $(SENTINAL)  help_info.txt  Makefile
	objcopy  $(RESOPTS) help_info.txt      $(OBJ_DIR)/help_info.o

$(OBJ_DIR)/help_ecce.o : $(SENTINAL)  help_ecce.txt  Makefile
	objcopy  $(RESOPTS) help_ecce.txt      $(OBJ_DIR)/help_ecce.o

$(OBJ_DIR)/help_syntax.o : $(SENTINAL)  help_syntax.txt  Makefile
	objcopy  $(RESOPTS) help_syntax.txt    $(OBJ_DIR)/help_syntax.o

$(OBJ_DIR)/help_usage.o : $(SENTINAL)  help_usage.txt  Makefile
	objcopy  $(RESOPTS) help_usage.txt     $(OBJ_DIR)/help_usage.o

$(OBJ_DIR)/license.o : $(SENTINAL)  LICENSE  Makefile
	objcopy  $(RESOPTS) LICENSE            $(OBJ_DIR)/license.o

$(OBJ_DIR)/warranty.o : $(SENTINAL)  warranty.txt  Makefile
	objcopy  $(RESOPTS) warranty.txt       $(OBJ_DIR)/warranty.o

# This is to make the directory, but the actual dependency is on the sentinal file
#
$(SENTINAL):
	@mkdir -p $(OBJ_DIR) && touch $(SENTINAL)

clean:
	rm -rf $(OBJ_DIR) *~

uninstall:
	rm -f $(TARGET)

# end
