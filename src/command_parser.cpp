/* command_parser.cpp
 *
 * This file is part of the ACE command line editor.
 *
 * Copyright (C) 1980-2022  Andrew C. Starritt
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

#include "command_parser.h"
#include "global.h"
#include <stdio.h>
#include <iostream>
#include <ctype.h>

#define ARRAY_LENGTH(xx)    static_cast<int> (sizeof (xx) /sizeof (xx [0]))

enum CommandKind {
   Forward = 0,
   Backward,
   Special,
   NUMBER_OF_KINDS
};

typedef BasicCommands BC;   // alias for brevity

// Maps command letter A for forms A, A- and %A to a basic command kind.
//
static const BasicCommands::Kinds charLookup [26][3] = {
   { BC::Absorbe,     BC::AbsorbeBack,    BC::Abandon        },
   { BC::BreakLine,   BC::BreakLineBack,  BC::Void           },
   { BC::Connect,     BC::Void,           BC::Close          },
   { BC::DeleteText,  BC::DeleteBack,     BC::Void           },
   { BC::Erase,       BC::EraseBack,      BC::Exchange       },
   { BC::Find,        BC::FindBack,       BC::Full           },
   { BC::Get,         BC::GetBack,        BC::Void           },
   { BC::UpperCase,   BC::LowerCase,      BC::Void           },
   { BC::Insert,      BC::InsertBack,     BC::Void           },
   { BC::Join,        BC::JoinBack,       BC::Void           },
   { BC::Kill,        BC::KillBack,       BC::Void           },
   { BC::Left,        BC::Void,           BC::LimitSet       },
   { BC::Move,        BC::MoveBack,       BC::Monitor        },
   { BC::Now,         BC::Void,           BC::Numbers        },
   { BC::Output,      BC::Void,           BC::Void           },
   { BC::Print,       BC::PrintBack,      BC::Prompt         },
   { BC::Quary,       BC::QuaryBack,      BC::Quiet          },
   { BC::Right,       BC::Void,           BC::RepeatSet      },
   { BC::Substitute,  BC::SubstituteBack, BC::SetMark        },
   { BC::Traverse,    BC::TraverseBack,   BC::TerminalMaxSet },
   { BC::Uncover,     BC::UncoverBack,    BC::Void           },
   { BC::Verify,      BC::VerifyBack,     BC::View           },
   { BC::Write,       BC::WriteBack,      BC::Void           },
   { BC::Void,        BC::Void,           BC::DefineX        },
   { BC::Void,        BC::Void,           BC::DefineY        },
   { BC::Void,        BC::Void,           BC::DefineZ        }
};

enum AllowedArgs {
   None = 0x00,
   Code =  0x01,   // exit code or other number
   Lim  =  0x02,   // search limit
   Txt  =  0x04,   // search text - required
   Rep  =  0x08,   // repeat limit - optional
   Mod  =  0x10    // modified - optional
};


struct CommandSpecification {
   const BasicCommands::Kinds kind;
   const std::string name;
   const unsigned allowed;
   const std::string description;
   const std::string failure;
};

// Note: This must be in th same order as BasicCommands::Kinds
//
static const CommandSpecification commandLookup [BC::NUMBER_OF_KINDS] = {
   { BC::Void,            "Void",            None,
     "n/a",  "n/a" },
   { BC::Absorbe,         "Absorbe",         Rep | Mod,
     "Absorbe a line from secondary input file.",
     "Secondary input file not opened or at end of input file." },
   { BC::BreakLine,       "BreakLine",       Rep | Mod,
     "Break current line (insert \\n) at cursor location.",
     "None." },
   { BC::Connect,         "Connect",         Txt | Mod,
     "Connect secondary input file specified by /text/.",
     "Specified file does not exist or not readable" },
   { BC::DeleteText,      "DeleteText",      Lim | Txt | Rep | Mod,
     "Delete next occurance of specified /text/.",
     "The text does not occur on the current line or at end of file." },
   { BC::Erase,           "Erase",           Rep | Mod,
     "Erase character to the immediate right of the cursor.",
     "Cursor is at end of line or at end of file." },
   { BC::Find,            "Find",            Lim | Txt | Rep | Mod,
     "Find next occurance of the specified /text/, cursor located to left of text.",
     "Specified text does not occur in rest of file." },
   { BC::Get,             "Get",             Rep | Mod,
     "Get a line from command input stream.",
     "Line consists of a single ':'. Usefull for g*" },
   { BC::UpperCase,       "UpperCase",       Rep | Mod,
     "Convert character to the immediate right of cursor to upper case\n"
     "and move cursor right one character.",
     "Cursor is at end of line or at end of file." },
   { BC::Insert,          "Insert",          Txt | Rep | Mod,
     "Insert specified text to the left of the cursor.",
     "Cursor is at end of file." },
   { BC::Join,            "Join",            Rep | Mod,
     "Joins the current line with the next line, is effectively D/\\n/.",
     "Cursor is on last line of file or at end of file." },
   { BC::Kill,            "Kill",            Rep | Mod,
     "Deletes the current line, cursor left at start of next line.",
     "Cursor is at end of file." },
   { BC::Left,            "Left",            Rep | Mod,
     "Move cursor left one character.",
     "Cursor is at start of line or at end of file." },
   { BC::Move,            "Move",            Rep | Mod,
     "Move cursor to the start of the next line.",
     "Cursor is at end of file." },
   { BC::Now,             "Now",             Rep | Mod,
     "Insert current date-time to the left of the cursor.",
     "Cursor is at end of file." },
   { BC::Output,          "Output",          Txt | Mod,
     "Connect secondary output file specified by /text/.",
     "Invlaid pathname/cannot create file." },
   { BC::Print,           "Print",           Rep | Mod,
     "Print current line to terminal.",
     "Cursor is at end of file." },
   { BC::Quary,           "Quary",           Rep | Mod,
     "Copies next <repeat> characters to the insert copy buffer.\n"
     "The cursor is not moved. Note (Q)5 is not the same as Q5.",
     "Less than <repeat> characters to the end of the line or\n"
     "cursor is at end of line or at end of file."},
   { BC::Right,           "Right",           Rep | Mod,
     "Move cursor right one character.",
     "Cursor is at end of line or at end of file." },
   { BC::Substitute,      "Substitute",      Txt | Mod,
     "Replace just found text with specified text, cursor\n"
     "is placed to the left of the replacement text.",
     "Previous Find, Uncover, or Verify command failed." },
   { BC::Traverse,        "Traverse",        Lim | Txt | Rep | Mod,
     "Find after next occurance of specified /text/, cursor located to right of text.",
     "Specified text does not occur in rest of line." },
   { BC::Uncover,         "Uncover",         Lim | Txt | Rep | Mod,
     "Uncover (remove) characters upto but not including specified /text/.",
     "The text does not occur on the current line or at end of file." },
   { BC::Verify,          "Verify",          Txt | Mod,
     "Compares the text to immediate right of cursor with specified /text/.",
     "The text to the immediate right of cursor does not match specified text." },
   { BC::Write,           "Write",           Rep | Mod,
     "Write current line to secondary output file.",
     "Secondary output file not open or at end of file." },

   // Reverse/backwards X- commands
   //
   { BC::AbsorbeBack,     "AbsorbeBack",     Rep | Mod,
     "Absorbe a line from secondary input file and move back 1 line.",
     "Secondary input file not opened." },
   { BC::BreakLineBack,   "BreakLineBack",   Rep | Mod,
     "Break current line (insert \\n) at cursor location.",
     "None." },
   { BC::DeleteBack,      "DeleteBack",      Lim | Txt | Rep | Mod,
     "Delete previous occurance of specified /text/.",
     "The text does not occur on the current line." },
   { BC::EraseBack,       "EraseBack",       Rep | Mod,
     "Erase character to the immediate left of the cursor.",
     "Cursor is at start of line or at end of file." },
   { BC::FindBack,        "FindBack",        Lim | Txt | Rep | Mod,
     "Find previous occurance of the specified /text/, cursor located to left of text.",
     "Specified text does not occur in file before current location." },
   { BC::GetBack,         "GetBack",         Rep | Mod,
     "Get a line from command input stream and move back 1 line.",
     "Line consists of a single ':'. Usefull for g*" },
   { BC::LowerCase,       "LowerCase",       Rep | Mod,
     "Convert character to the immediate right of cursor\n"
     "to lower case and move cursor right one character.",
     "Cursor is at end of line or at end of file." },
   { BC::InsertBack,      "InsertBack",      Txt | Rep | Mod,
     "Insert specified text to the right of the cursor.",
     "Cursor is at end of file." },
   { BC::JoinBack,        "JoinBack",        Rep | Mod,
     "Joins the previous line with the current line, effectively D-/\\n/.",
     "Cursor is on first line of file or at end of file." },
   { BC::KillBack,        "KillBack",        Rep | Mod,
     "Deletes the previous line (K as in Kill), cursor left\n"
     "at start of the current line.",
     "Cursor is on first line of the file." },
   { BC::MoveBack,        "MoveBack",        Rep | Mod,
     "Move cursor to the start of the previous line.",
     "Cursor is at start of file." },
   { BC::PrintBack,       "PrintBack",       Rep | Mod,
     "Print current line to terminal.",
     "Cursor is at end of file." },
   { BC::QuaryBack,       "QuaryBack",       Rep | Mod,
     "Copies next <repeat> characters to the insert copy buffer.\n"
     "The cursor is not moved. Note (Q)5 is not the same as Q5.",
     "Less than <repeat> characters to the end of the line or\n"
     "cursor is at end of line or at end of file."},
   { BC::SubstituteBack,  "SubstituteBack",  Txt | Mod,
     "???",
     "???" },
   { BC::TraverseBack,    "TraverseBack",    Lim | Txt | Rep | Mod,
     "Find after previous occurance of specified /text/, cursor located to right of text.",
     "Specified text does not occur in file before current location." },
   { BC::UncoverBack,     "UncoverBack",     Lim | Txt | Rep | Mod,
     "Uncover (remove) characters upto and including previous /text/.",
     "The text does not occur on the current line." },
   { BC::VerifyBack,      "VerifyBack",      Txt | Mod,
     "Verifiy that the text in the file to the immediate\n"
     "left of the cursor is the same as the specified text.",
     "The text does not match (check is case sensitive)." },
   { BC::WriteBack,        "WriteBack",        Rep | Mod,
     "Write current line to secondary output file.",
     "Secondary output file not open or at end of file." },

   // Special %X commands
   //
   { BC::Abandon,         "Abandon",         Lim,
     "Abandon edit session (exit code 1).",
     "None." },
   { BC::Close,           "Close",           Lim,
     "Close edit session (exit code 0).",
     "None." },
   { BC::Exchange,        "Exchange",        None,
     "Swap (Exchange) the last found and last inserted strings.",
     "None." },
   { BC::Full,            "Full",            None,
     "Full monitoring mode - current line printed after every command line.",
     "None." },
   { BC::LimitSet,        "LimitSet",        Lim,
     "Set interpretation of * for search limit - default is 100000.",
     "" },
   { BC::Monitor,         "Monitor",         None,
     "Normal monitoring mode - current line printed only if has been modified.",
     "None." },
   { BC::Numbers,         "line Numbers",    None,
     "Toggle on/off displaying line numbers.",
     "None." },
   { BC::Prompt,          "Prompt",          None,
     "Toggle off/on the command prompts (>, :, X?, Y?, and/or Z?).",
     "None." },
   { BC::Quiet,           "Quiet",           None,
     "Quiet monitoring mode - current line never printed after command line.",
     "None." },
   { BC::RepeatSet,       "RepeatSet",       Lim,
     "Set the repitition limit used for '*' and '0' - default is 50000.",
     "None." },
   { BC::SetMark,         "SetMark",         Txt,
     "Set the cursor indication character to first character of the specified string",
     "None." },
   { BC::TerminalMaxSet,  "TerminalMaxSet",  Lim,
     "Set the command line terminal width (min is 32) - default is 160.",
     "None." },
   { BC::View,            "View",            Lim,
     "View current settings - increase value for more detail.",
     "None." },
   { BC::DefineX,         "DefineX",         Txt,
     "Define the X macro - if /text/ is empty, the user is prompted.",
     "None." },
   { BC::DefineY,         "DefineY",         Txt,
     "Define the Y macro - if /text/ is empty, the user is prompted.",
     "None." },
   { BC::DefineZ,         "DefineZ",         Txt,
     "Define the Z macro - if /text/ is empty, the user is prompted.",
     "None." }
};

// Perfrom a commandLookup sanity check
//
static bool _check() {

   for (int j = 0; j < ARRAY_LENGTH (commandLookup); j++) {
      if (j != commandLookup[j].kind) {
         std::cout << "*** commandLookup[" << j << "].kind miss match: "
                   << commandLookup[j].kind << std::endl;
      }
   }

   return true;
}

static bool _checked = _check ();

static const int magicNoInt = -1;
static const int magicAMTAP = 0;    // As many times as possible.

//------------------------------------------------------------------------------
//
static int indexOfLetter (const char letter)
{
   int result;

   if ((letter >= 'A') && (letter <= 'Z')) {
      result = int(letter) - int('A');
   } else  if ((letter >= 'a') && (letter <= 'z')) {
      result = int(letter) - int('a');
   } else {
      result = -1;
   }
   return result;
}

//------------------------------------------------------------------------------
//
static std::string reverseLookUp (const BasicCommands::Kinds kind)
{
   char letter = ' ';
   bool back = false;
   bool special = false;

   for (int i = 0; i < ARRAY_LENGTH (charLookup); i++) {
      for (int j = 0; j < ARRAY_LENGTH (charLookup[i]); j++) {
         if (charLookup[i][j] == kind) {
            // Found it.
            //
            letter = char (int('A') + i);
            back = (j == 1);
            special = (j == 2);
            break;
         }
      }
   }

   std::string result = "";
   if (special) result += '%';
   result += letter;
   if (back) result += '-';

   return result;
}

//------------------------------------------------------------------------------
//
static bool caseInSensCompareChar (const char& c1, const char& c2)
{
   if (c1 == c2)
      return true;
   else if (std::toupper(c1) == std::toupper(c2))
      return true;
   return false;
}

//------------------------------------------------------------------------------
//
static bool caseInSensCompareString (const std::string& str1,
                                     const std::string& str2)
{
   return ( (str1.size() == str2.size()) &&
            std::equal(str1.begin(), str1.end(),
                       str2.begin(), &caseInSensCompareChar) );
}

//------------------------------------------------------------------------------
// static
std::string CommandParser::name (const BasicCommands::Kinds kind)
{
   return commandLookup[kind].name;
}

//------------------------------------------------------------------------------
// static
void CommandParser::commandHelp (const BasicCommands::Kinds kind,
                                 std::ostream& stream)
{
   stream << commandLookup[kind].name  << ": "
          << commandLookup[kind].description << std::endl;

   std::string syntax = reverseLookUp (kind);

   const unsigned allowed = commandLookup[kind].allowed;

   if (allowed & Code) syntax += " [Number]";
   if (allowed & Lim) syntax += " [Limit]";
   if (allowed & Txt) syntax += " {/text/,&}";
   if (allowed & Rep) syntax += " [Repeat]";
   if (allowed & Mod) syntax += " [{@|?|\\|~}]";

   stream << "Syntax: " << syntax << std::endl;
   stream << "Failure condition: "
          << commandLookup[kind].failure << std::endl;
   stream << std::endl;
}

//------------------------------------------------------------------------------
// static
void CommandParser::commandSummary (std::ostream& stream)
{
   const int numberCols = 4;
   const int numberRows = (ARRAY_LENGTH (commandLookup) + numberCols - 1)/ numberCols;

   for (int r = 0; r < numberRows; r++) {
      for (int c = 0; c < numberCols; c++) {
         const int k = 1 + r + numberRows * c;
         if (k >= ARRAY_LENGTH (commandLookup)) break;

         char buffer [28];
         const BasicCommands::Kinds kind = static_cast<BasicCommands::Kinds> (k);
         snprintf (buffer, sizeof (buffer), "%-2s : %-19s",
                   reverseLookUp (kind).c_str(),
                   commandLookup[kind].name.c_str());

         stream << buffer;
      }
      stream << std::endl;
   }

   // Pseudo commands
   //
   stream << "(  : Start Compound     ";
   stream << ")  : End Compound       ";
   stream << ",  : Alternative" << std::endl;

}

//------------------------------------------------------------------------------
// static
void CommandParser::help (const std::string& item, std::ostream& stream)
{
   BasicCommands::Kinds kind = BasicCommands::Void;

   // Do a few special cases first
   //
   if (item == "(") {
      stream << "Start Compound: Start of compound command." << std::endl;
      stream << "Syntax: (" << std::endl;
      stream << "Failure condition: Too deep within compound commands" << std::endl;
      stream << std::endl;
      return;
   }

   if (item == ")") {
      stream << "End Compound: End of compound command." << std::endl;
      stream << "Syntax: ) [Repeat] [{@,?,\\,~,}]" << std::endl;
      stream << "Failure condition: Unmatched to a start compound command." << std::endl;
      stream << std::endl;
      return;
   }

   if (item == ",") {
      stream << "Alternative: Introduces alternative command sequence." << std::endl;
      stream << "Syntax: ," << std::endl;
      stream << "Failure condition: None." << std::endl;
      stream << std::endl;
      return;
   }

   if (item.size() == 1) {
      const int j = indexOfLetter (item [0]);
      if (j >= 0) {
         kind = charLookup [j][0];
      }
   }
   else if ((item.size() == 2) && (item [1] == '-')) {
      const int j = indexOfLetter (item [0]);
      if (j >= 0) {
         kind = charLookup [j][1];
      }
   }
   else if ((item.size() == 2) && (item [0] == '%')) {
      const int j = indexOfLetter (item [1]);
      if (j >= 0) {
         kind = charLookup [j][2];
      }
   } else {
      for (int j = 1; j < ARRAY_LENGTH (commandLookup); j++) {
         if (caseInSensCompareString (item, commandLookup[j].name)) {
            kind = commandLookup[j].kind;
            break;
         }
      }
   }

   if (kind != BasicCommands::Void) {
      CommandParser::commandHelp (kind, stream);
   } else {
      stream << "Unknown command: " << item << std::endl;
      stream << std::endl;
   }
}

//------------------------------------------------------------------------------
//
CommandParser::CommandParser() { }

//------------------------------------------------------------------------------
//
CommandParser::~CommandParser() { }

//------------------------------------------------------------------------------
//
CompoundCommands* CommandParser::parse (const std::string commandLine)
{
   CompoundCommands* result = nullptr;
   int last = 0;
   int brackets = 0;

   // Macro replacement
   // Replace content in the line any unquoted ('"/! etc.) X, Y and/or Z
   // Rince and repeat to 4 times - we have three macros.
   //
   std::string workLine = commandLine;

   for (int k = 1; k <= 4; k++) {

      bool isBetweenQuotes = false;
      char quote = '\0';
      char prevChar = '\0';

      std::string::size_type ptr = 0;
      while (ptr < workLine.length()) {
         char x = workLine.at (ptr++);

         if (isBetweenQuotes) {
            // We are currently quoted - all we need to worry about is the unquote.
            //
            if (x == quote) {
               isBetweenQuotes = false;
            }

         } else {
            // Not currently quoted
            //
            char m = toupper (x);
            if ((prevChar != '%') && (m == 'X' || m == 'Y' || m == 'Z')) {

               // unquoted macro call - do replacements
               //
               if (k >= 4) {
                  // Forth time around the loop - we should have been
                  // all done by the end of the 3rd loop.
                  //
                  std::cerr << "Recursive macro expansion - command line ignored" << std::endl;
                  return nullptr;
               }

               std::string replacement;
               switch (m) {
                  case 'X': replacement = Global::getMacroX(); break;
                  case 'Y': replacement = Global::getMacroY(); break;
                  case 'Z': replacement = Global::getMacroZ(); break;
               }

               std::string part1 = workLine.substr (0, ptr - 1);
               std::string part2 = workLine.substr (ptr);    // to end of line

               workLine = part1 + replacement + part2;
               ptr = ptr - 1 + replacement.length();

               if ((ptr > 0) && (ptr <= workLine.size())) {
                  prevChar = workLine.at (ptr - 1);
               } else {
                  prevChar = '\0';
               }
            }

            else if (CommandParser::isQuote(x)) {
               isBetweenQuotes = true;
               quote = x;
               prevChar = x;

            } else {
               // Just an "ordinary" character - no need to do anything.
               //
               prevChar = x;
            }
         }
      }
   }

   result = this->parse (workLine, last, brackets);

   if (result && (brackets > 0)) {
      std::cerr << "Unmatched ( - line ignored" << std::endl;
      delete result;
      result = nullptr;
   }
   else if (result && (brackets < 0)) {
      std::cerr << "Unmatched ) - line ignored" << std::endl;
      delete result;
      result = nullptr;
   }

   return result;
}

//------------------------------------------------------------------------------
// Friendly wrapper macros around these functions.
// Quazi local functions.
//
#define NEXT_CHAR()      CommandParser::nextChar   (commandLine, ptr)
#define SKIP_SPACES()    CommandParser::skipSpaces (commandLine, ptr)
#define GET_INT()        CommandParser::getInt     (commandLine, ptr)
#define GET_STR(okay)    CommandParser::getStr     (commandLine, ptr, okay)
#define GET_MOD()        CommandParser::getMod     (commandLine, ptr)


//------------------------------------------------------------------------------
// Note: this is called recusively for compound commands.
//
CompoundCommands* CommandParser::parse (const std::string commandLine,
                                        int& last, int& brackets)
{
   CompoundCommands* result = nullptr;
   last = 0;   // ensure not erroneous

   const int len = commandLine.length();

   Sequences seq;
   Alternatives alt;
   AbstractCommands* command = nullptr;

   int ptr = 0;
   while (ptr < len) {
      char x = commandLine.at (ptr++);

      x = toupper (x);

      if (x == ' ') continue;
      if (x == '#' || x == ';') break;

      if (x == '(') {
         brackets++;
         std::string subLine = commandLine.substr (ptr);
         int relative;

         command = parse (subLine, relative, brackets);
         if (!command) {
            clearSequence (seq);
            return nullptr;
         }

         ptr += relative;  // carry on past compound command.
         alt.push_back (command);
      }

      else if (x == ',') {
         seq.push_back (alt);
         alt.clear();
      }

      else if (x == ')') {
         brackets--;

         // Set defaults
         //
         int repeats = 1;
         AbstractCommands::Modifiers modifier = AbstractCommands::Normal;

         // Get number of repeats and the modifier if any.
         //
         const int temp = GET_INT();  // only none -ve numbers are read.

         if (temp != magicNoInt) {
            // we read an integer (includs the * number)
            //
            if (temp == magicAMTAP) {
               modifier = AbstractCommands::AMTAP;
            }
            repeats = temp;   // essentially ignored when modifier is AMTAP
         }

         if (temp != magicAMTAP) {
            // We do not allow A*@ or A*~
            //
            modifier = GET_MOD();
         }

         last = ptr;  // update where we got to.
         seq.push_back (alt);

         result = new CompoundCommands (seq, repeats, modifier);
         return result;
      }

      else if ((x == '%') || (x >= 'A' && x <= 'Z')) {
         // Allow commands are A, A- and %A.

         std::string name;
         CommandKind col = Forward;   // default

         if (x == '%') {
            name.push_back (x);
            SKIP_SPACES();
            col = Special;
            x = NEXT_CHAR();
            if (x != '\0') {
               // Is an actual char.
               //
               ptr++;  // "read" it
               x = toupper (x);
            }
         }

         if (x >= 'A' && x <= 'Z') {
            name.push_back (x);

            if ((col == Forward) && (NEXT_CHAR() == '-')) {
               ptr++;
               col = Backward;
               name.push_back('-');
            }

            int row = x - 'A';
            BasicCommands::Kinds kind = charLookup [row][col];

            if (kind != BasicCommands::Void) {
               // Set defaults
               //
               int limit = 1;
               if (kind == BasicCommands::Find ||
                   kind == BasicCommands::FindBack) limit = 0;  // magic value for search limits

               if (kind == BasicCommands::Close)    limit = 0;
               if (kind == BasicCommands::Abandon)  limit = 2;  // abort exit code
               if (kind == BasicCommands::View)     limit = 0;

               std::string text;
               bool useLastText = false;
               int repeats = 1;
               AbstractCommands::Modifiers modifier = AbstractCommands::Normal;

               const unsigned int allowed =  commandLookup [kind].allowed;

               if (allowed & Lim) {
                  const int temp = GET_INT ();
                  if (temp != magicNoInt) {
                     limit = temp;
                  }
               }

               if (allowed & Txt) {
                  SKIP_SPACES();
                  if (NEXT_CHAR() == '&') {
                     ptr++;  // read the "&"
                     useLastText = true;
                  } else {
                     bool okay;
                     text = GET_STR(okay);
                     if (!okay) {
                        std::cerr << "Missing string " << name << std::endl;
                        clearSequence (seq);
                        return nullptr;
                     }
                  }
               }

               // Get number of repeats and the modifier if any.
               //
               int temp = magicNoInt;

               // Get number of repeats and the modifier if any.
               //
               if (allowed & Rep) {
                   temp = GET_INT ();  // only non -ve numbers are read.
               }

               if (temp != magicNoInt) {
                  // we read an integer (including the * number)
                  //
                  if (temp == magicAMTAP) {
                     modifier = AbstractCommands::AMTAP;
                  }
                  repeats = temp;   // essentially ignored when modifier is AMTAP
               }

               if ((allowed & Mod) && (temp != magicAMTAP)) {
                  // We do not allow e.g. A*@ or A*~
                  // * is a quazi modifier.
                  //
                  modifier = GET_MOD();
               }

               command = new BasicCommands (kind, modifier, limit, repeats, text, useLastText);
               alt.push_back (command);

            } else {
               std::cerr << "Unknown command: " << name << " line ignored" << std::endl;
               clearSequence (seq);
               return nullptr;
            }

         } else {
            std::cerr << "Unknown command: " << name << " line ignored" << std::endl;
            clearSequence (seq);
            return nullptr;
         }

      } else {
         std::cerr << "Unknown command: " << x << " line ignored" << std::endl;
         clearSequence (seq);
         return nullptr;
      }
   }

   last = ptr;

   seq.push_back (alt);
   alt.clear();

   result = new CompoundCommands (seq, 1, AbstractCommands::Normal);
   return result;
}

//------------------------------------------------------------------------------
// static
char CommandParser::nextChar (const std::string commandLine, int& ptr)
{
   const int len = commandLine.length();
   return (ptr < len) ? commandLine.at (ptr) : '\0';
}

//------------------------------------------------------------------------------
// static
void CommandParser::skipSpaces (const std::string commandLine, int& ptr)
{
   char n;
   n = nextChar (commandLine, ptr);
   while (n == ' ' || n == '\t') {
      ptr++;
      n = nextChar (commandLine, ptr);
   }
}

//------------------------------------------------------------------------------
// static
int CommandParser::getInt (const std::string commandLine, int& ptr)
{
   int result = magicNoInt;
   char x;

   skipSpaces (commandLine, ptr);

   x = CommandParser::nextChar (commandLine, ptr);
   if ('0' <= x && x <= '9') {
      // Some short of number has been specified.
      //
      result = 0;
      do {
         ptr++;  // "read" the next char
         int d = x - '0';
         result = 10* result + d;
         x = CommandParser::nextChar (commandLine, ptr);
      } while ('0' <= x && x <= '9');
   }
   else if (x == '*') {
      ptr++;  // "read" the * char
      result = magicAMTAP;  // the magic value for as many times as possible.
   }

   return result;
}

//------------------------------------------------------------------------------
// Defines the allowed quote characters
// static
bool CommandParser::isQuote (const char x)
{
   return ((x == '"') || (x == '/') || (x == '\'') ||
           (x == '!') || (x == '.') || (x == '+') ||
           (x == '`') || (x == ':') || (x == '=') ||
           (x == '|') || (x == '^') );
}

//------------------------------------------------------------------------------
// static
std::string
CommandParser::getStr (const std::string commandLine, int& ptr, bool& okay)
{
   std::string result;
   okay = false;

   skipSpaces (commandLine, ptr);

   // Check for allowed quote characters are:
   //
   const char x = CommandParser::nextChar (commandLine, ptr);
   if (CommandParser::isQuote (x)) {
      ptr++;  // "read" the start quote char

      // Simple string match.
      // No escaping the quote using. e.g. \" or allowing "" to mean ".
      //
      const char quote = x;

      char n = CommandParser::nextChar (commandLine, ptr);
      while (n != quote && n != '\0') {
         ptr++;  // "read" the string char
         result.push_back (n);
         n = CommandParser::nextChar (commandLine, ptr);
      }

      if (n == quote) {
         ptr++;  // "read" the end quote char
      }

      okay = true;
   }

   return result;
}

//------------------------------------------------------------------------------
// static
AbstractCommands::Modifiers
CommandParser::getMod (const std::string commandLine, int& ptr)
{
   AbstractCommands::Modifiers result = AbstractCommands::Normal;

   skipSpaces (commandLine, ptr);

   const char n = CommandParser::nextChar (commandLine, ptr);

   switch (n) {
      case '?':
      case '@':
         result = AbstractCommands::NoFail;
         ptr++;
         break;

      case '\\':
      case '~':
         result = AbstractCommands::Invert;
         ptr++;
         break;
   }

   return result;
}

// end
