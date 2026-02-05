/* commands.h
 *
 * This file is part of the ACE command line editor.
 *
 * SPDX-FileCopyrightText: 1980-2026  Andrew C. Starritt
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Contact details:
 * andrew.starritt@gmail.com
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
   // Controls how a command execution status is modified.
   //
   enum Modifiers {
      Normal,  // No modification.
      AMTAP,   // As many times as possible without failure.
               // In practice limited to 10K to avoid infinite loops.
      NoFail,  // ? or @ qualifier
      Invert   // \ or ~ qualifier
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
class BasicCommands : public AbstractCommands {
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
      NowBack,
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
      Backup,
      Close,
      DelimiterSmart,
      Exchange,
      Full,
      Intermediate,
      LimitSet,
      Monitor,
      Numbers,  // as in Line Numbers
      Prompt,
      Quiet,
      RepeatSet,
      SetCursorMark,
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

   Kinds getKind() const;
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
// Maybe this should be a class in own right.
//
typedef std::list <AbstractCommands*> Alternatives;
typedef std::list <Alternatives> Sequences;

void clearSequence (Sequences& sequence);

//------------------------------------------------------------------------------
//
class CompoundCommands : public AbstractCommands {
public:
   explicit CompoundCommands (const Sequences sequence,
                              const int number, const Modifiers modifier);
   ~CompoundCommands ();

   std::string image () const;
   bool execute (DataBuffer& db);

   AbstractCommands* getLastCommand() const;  // can be nullptr
   AbstractCommands* getLastSuccessfullCommand() const;  // can be nullptr

private:
   Sequences sequence;
   AbstractCommands* lastCommand;
   AbstractCommands* lastSuccessfullCommand;
};

#endif // ACE_COMMANDS_H
