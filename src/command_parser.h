/* command_parser.
 *
 * This file is part of the ACE command line editor.
 *
 * SPDX-FileCopyrightText: 1980-2026  Andrew C. Starritt
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Contact details:
 * andrew.starritt@gmail.com
 */

#ifndef ACE_COMMAND_PARSER_H
#define ACE_COMMAND_PARSER_H

#include <iostream>
#include <string>
#include "commands.h"

// All function (currently) static.
// Make it a namespace ?
//
class CommandParser
{
public:
   static std::string name (const BasicCommands::Kinds kind);

   static void commandHelp (const BasicCommands::Kinds kind,
                            std::ostream& stream);

   static void commandSummary (std::ostream& stream);

   static void help (const std::string& item,
                     std::ostream& stream);

   // Returns nullptr on parse failure.
   // Returned object must be deleted to avoid memory loss.
   // Make static ??
   //
   static CompoundCommands* parse (const std::string commandLine);

private:
   // Don't allow a CommandParser object to be constructed.
   //
   explicit CommandParser();
   ~CommandParser();

   // Returns nullptr on parse failure.
   //
   static CompoundCommands* parseLine (const std::string commandLine,
                                       int& last, int& brackets);

   static char nextChar      (const std::string commandLine, const int ptr);
   static char readChar      (const std::string commandLine, int& ptr);
   static void skipSpaces    (const std::string commandLine, int& ptr);
   static int  getInt        (const std::string commandLine, int& ptr);
   static bool isQuote       (const char x);
   static bool isSmartQuote  (const char x);
   static std::string getStr (const std::string commandLine, int& ptr, bool& okay);
   static AbstractCommands::Modifiers getMod (const std::string commandLine, int& ptr);

   friend class Global;
};

#endif // ACE_COMMAND_PARSER_H
