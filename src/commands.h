/* commands.h
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

#ifndef ACE_COMMANDS_H
#define ACE_COMMANDS_H

#include <string>
#include <list>

class DataBuffer;   // differed

//------------------------------------------------------------------------------
//
class AbstractCommands {
public:

   enum Modifiers {
      Normal,
      AMTAP,   // As many times as possible without failure.
               // In practice limited to 10K to avoid infinite loops.
      NoFail,  // ? or @
      Invert   // \ or ~
   };

   explicit AbstractCommands (const int number, const Modifiers modifier);
   virtual ~AbstractCommands ();

   virtual std::string image () const;
   virtual bool execute (DataBuffer& db) = 0;

protected:
   bool twizzle (const bool status) const;
   const int number;
   const Modifiers modifier;
};


//------------------------------------------------------------------------------
//
class BasicCommands : public AbstractCommands
{
public:
   enum Kinds {
      Void = 0,
      Absorbe,
      BreakLine,
      Connect,
      DeleteText,
      Erase,
      Find,
      Get,
      UpperCase,
      Insert,
      Join,
      Kill,
      Left,
      Move,
      Now,
      Output,
      Print,
      Quary,
      Right,
      Substitute,
      Traverse,
      Uncover,
      Verify,
      Write,

      // Reverse/backwards X- commands
      //
      AbsorbeBack,
      BreakLineBack,
      DeleteBack,
      EraseBack,
      FindBack,
      GetBack,
      LowerCase,
      InsertBack,
      JoinBack,
      KillBack,
      MoveBack,
      PrintBack,
      QuaryBack,
      SubstituteBack,
      TraverseBack,
      UncoverBack,
      VerifyBack,
      WriteBack,

      // Special %X commands
      //
      Abandon,
      Close,
      Exchange,
      Full,
      LimitSet,
      Monitor,
      Numbers,  // Line Numbers
      Prompt,
      Quiet,
      RepeatSet,
      SetMark,
      TerminalMaxSet,
      View,
      DefineX,
      DefineY,
      DefineZ,
      NUMBER_OF_KINDS   // must be last
   };

   explicit BasicCommands (const Kinds kind, const Modifiers modifier,
                           const int limit,  const int number,
                           const std::string text, const bool useLastText);
   virtual ~BasicCommands();

   std::string image () const;
   bool execute (DataBuffer& db);

private:
   const Kinds kind;
   const int limit;         // also exit code for %C and %A, verbosity fotr %V
   const bool useLastText;
   const std::string text;
   std::string useText;     // actual text used for search, modify or filename.
};

//------------------------------------------------------------------------------
// A compound command is a sequence basic commands together with zero or more
// alternative command sequences in case of failure. It is implemented as a list
// of lists of AbstractCommands.
//
typedef std::list <AbstractCommands*> Alternatives;
typedef std::list <Alternatives> Sequences;

void clearSequence (Sequences& sequence);

class CompoundCommands : public AbstractCommands {
public:
   explicit CompoundCommands (const Sequences sequence,
                              const int number, const Modifiers modifier);
   ~CompoundCommands ();

   std::string image () const;
   bool execute (DataBuffer& db);

   AbstractCommands* getLastCommand() const;  // can be nullptr

private:
   Sequences sequence;
   AbstractCommands* lastCommand;
};

#endif // ACE_COMMANDS_H
