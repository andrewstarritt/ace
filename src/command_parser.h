/* command_parser.
 *
 * This file is part of the ACE command line editor.
 *
 * Copyright (C) 1980-2021  Andrew C. Starritt
 *
 * The ACE program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * The ACE program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with the ACE program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact details:
 * andrew.starritt@gmail.com
 * PO Box 3118, Prahran East, Victoria 3181, Australia.
 */

#ifndef ACE_COMMAND_PARSER_H
#define ACE_COMMAND_PARSER_H

#include <iostream>
#include <string>
#include "commands.h"

class CommandParser
{
public:
   static std::string name (const BasicCommands::Kinds kind);

   static void commandHelp (const BasicCommands::Kinds kind,
                            std::ostream& stream);

   static void commandSummary (std::ostream& stream);

   static void help (const std::string& item,
                     std::ostream& stream);

   explicit CommandParser();
   ~CommandParser();

   // Returns NULL nullptr on failure.
   // Returned object must be deleted to avoid memory loss.
   // Make static ??
   //
   CompoundCommands* parse (const std::string commandLine);

private:
   CompoundCommands* parse (const std::string commandLine, int& last, int& brackets);

   static char nextChar      (const std::string commandLine, int& ptr);
   static void skipSpaces    (const std::string commandLine, int& ptr);
   static int  getInt        (const std::string commandLine, int& ptr);
   static bool isQuote       (const char x);
   static std::string getStr (const std::string commandLine, int& ptr, bool& okay);
   static AbstractCommands::Modifiers getMod (const std::string commandLine, int& ptr);
};

#endif // ACE_COMMAND_PARSER_H
