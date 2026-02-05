/* commands.cpp
 *
 * This file is part of the ACE command line editor.
 *
 * SPDX-FileCopyrightText: 1980-2026  Andrew C. Starritt
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Contact details:
 * andrew.starritt@gmail.com
 */

#include "commands.h"
#include "command_parser.h"
#include "data_buffer.h"
#include "global.h"
#include <iostream>

//==============================================================================
// AbtractActions
//==============================================================================
//
AbstractCommands::AbstractCommands (const int numberIn,
                                    const Modifiers modifierIn) :
   number (numberIn),
   modifier (modifierIn)
{ }

//------------------------------------------------------------------------------
//
AbstractCommands::~AbstractCommands () { }

//------------------------------------------------------------------------------
// virtual
std::string AbstractCommands::image () const
{
   std::string result = "None";
   return result;
}

//------------------------------------------------------------------------------
//
bool AbstractCommands::twizzle (const bool status) const
{
   bool result;

   switch (this->modifier) {
      case Normal:
         result = status;
         break;

      case AMTAP:
      case NoFail:
         result = true;
         break;

      case Invert:
         result = !status;
         break;

      default:
         result = false;
   }

   return result;
}


//==============================================================================
// BasicCommands
//==============================================================================
//
BasicCommands::BasicCommands (const Kinds kindIn, const Modifiers modifierIn,
                              const int limitIn, const int numberIn,
                              const std::string textIn, const bool useLastTextIn) :
   AbstractCommands (numberIn, modifierIn),
   kind (kindIn),
   limit (limitIn),
   useLastText (useLastTextIn),
   text (textIn)
{ }

//------------------------------------------------------------------------------
//
BasicCommands::~BasicCommands()
{ }

//------------------------------------------------------------------------------
//
BasicCommands::Kinds BasicCommands::getKind() const {
   return this->kind;
}

//------------------------------------------------------------------------------
// override
std::string BasicCommands::image () const
{
   std::string result;

   switch (this->kind) {
      // Text based commands
      //
      case Connect:
      case DeleteText:
      case Find:
      case Insert:
      case Output:
      case Substitute:
      case Traverse:
      case Uncover:
      case Verify:
      case DeleteBack:
      case EraseBack:
      case FindBack:
      case SubstituteBack:
      case TraverseBack:
      case UncoverBack:
      case VerifyBack:
         result = CommandParser::name (this->kind) + " '" + this->useText + "'";
         break;

      // Everything else
      //
      default:
         result = CommandParser::name (this->kind);
         break;
   }

   return result;
}

//------------------------------------------------------------------------------
//
bool BasicCommands::execute (DataBuffer& db)
{
   bool result = false;

   // Zero implies the current extended search limit.
   const int useLimit  = this->limit    == 0     ? Global::getSearchMax() : this->limit;
   const int useRepeat = this->modifier == AMTAP ? Global::getRepeatMax() : this->number;

   switch (this->kind) {
      case Absorbe:
         result = db.absorbe (useRepeat);
         break;

      case BreakLine:
         result = db.breakLine (useRepeat);
         break;

      case Connect:
         this->useText = this->useLastText ? Global::getLastFilename() : this->text;
         Global::setLastFilename (this->useText);
         result = db.connect (useText);
         break;

      case DeleteText:
         this->useText = this->useLastText ? Global::getLastSearch() : this->text;
         Global::setLastSearch (this->useText);
         result = db.deleteText (useLimit, useText, useRepeat);
         break;

      case Erase:
         result = db.erase (useRepeat);
         break;

      case Find:
         this->useText = this->useLastText ? Global::getLastSearch() : this->text;
         Global::setLastSearch (this->useText);
         result = db.find (useLimit, useText, useRepeat);
         break;

      case Get:
         result = db.get (useRepeat);
         break;

      case UpperCase:
         result = db.upperCase (useRepeat);
         break;

      case Insert:
         this->useText = this->useLastText ? Global::getLastModify() : this->text;
         Global::setLastModify (this->useText);
         result = db.insert (useText, useRepeat);
         break;

      case Join:
         result = db.join (useRepeat);
         break;

      case Kill:
         result = db.kill (useRepeat);
         break;

      case Left:
         result = db.left (useRepeat);
         break;

      case Move:
         result = db.move (useRepeat);
         break;

      case Now:
         result = db.now (useRepeat);
         break;

      case Output:
         this->useText = this->useLastText ? Global::getLastFilename() : this->text;
         Global::setLastFilename (this->useText);
         result = db.output (useText);
         break;

      case Print:
         result = db.print (useRepeat);
         break;

      case Quary:
         result = db.quary (useRepeat);
         break;

      case Right:
         result = db.right (useRepeat);
         break;

      case Substitute:
         this->useText = this->useLastText ? Global::getLastModify() : this->text;
         Global::setLastModify (this->useText);
         result = db.substitute (useText, useRepeat);
         break;

      case Traverse:
         this->useText = this->useLastText ? Global::getLastSearch() : this->text;
         Global::setLastSearch (this->useText);
         result = db.traverse (useLimit, useText, useRepeat);
         break;

      case Uncover:
         this->useText = this->useLastText ? Global::getLastSearch() : this->text;
         Global::setLastSearch (this->useText);
         result = db.uncover (useLimit, useText, useRepeat);
         break;

      case Verify:
         this->useText = this->useLastText ? Global::getLastSearch() : this->text;
         Global::setLastSearch (this->useText);
         result = db.verify (useText);
         break;

      case Write:
         result = db.write (useRepeat);
         break;


         /// Reverse/backwards X- commands
         ///
      case AbsorbeBack:
         result = db.absorbeBack (useRepeat);
         break;

      case BreakLineBack:
         result = db.breakLineBack (useRepeat);
         break;

      case DeleteBack:
         this->useText = this->useLastText ? Global::getLastSearch() : this->text;
         Global::setLastSearch (this->useText);
         result = db.deleteBack (useLimit, useText, useRepeat);
         break;

      case EraseBack:
         result = db.eraseBack (useRepeat);
         break;

      case FindBack:
         this->useText = this->useLastText ? Global::getLastSearch() : this->text;
         Global::setLastSearch (this->useText);
         result = db.findBack (useLimit, useText, useRepeat);
         break;

      case GetBack:
         result = db.getBack (useRepeat);
         break;

      case LowerCase:
         result = db.lowerCase (useRepeat);
         break;

      case InsertBack:
         this->useText = this->useLastText ? Global::getLastModify() : this->text;
         Global::setLastModify (this->useText);
         result = db.insertBack (useText, useRepeat);
         break;

      case JoinBack:
         result = db.joinBack (useRepeat);
         break;

      case KillBack:
         result = db.killBack (useRepeat);
         break;

      case MoveBack:
         result = db.moveBack (useRepeat);
         break;

      case NowBack:
         result = db.nowBack (useRepeat);
         break;

      case PrintBack:
         result = db.printBack (useRepeat);
         break;

      case QuaryBack:
         result = db.quaryBack (useRepeat);
         break;

      case SubstituteBack:
         this->useText = this->useLastText ? Global::getLastModify() : this->text;
         Global::setLastModify (this->useText);
         result = db.substituteBack (useText, useRepeat);
         break;

      case TraverseBack:
         this->useText = this->useLastText ? Global::getLastSearch() : this->text;
         Global::setLastSearch (this->useText);
         result = db.traverseBack (useLimit, useText, useRepeat);
         break;

      case UncoverBack:
         this->useText = this->useLastText ? Global::getLastSearch() : this->text;
         Global::setLastSearch (this->useText);
         result = db.uncoverBack (useLimit, useText, useRepeat);
         break;

      case VerifyBack:
         this->useText = this->useLastText ? Global::getLastSearch() : this->text;
         Global::setLastSearch (this->useText);
         result = db.verifyBack (useText);
         break;

      case WriteBack:
         result = db.writeBack (useRepeat);
         break;


         /// Special commands
         ///
      case Abandon:
         Global::requestAbandon (this->limit);
         result = true;
         break;

      case Backup:
         if (Global::getTargetFilename() != DataBuffer::stdInOut()){
            result = db.save(Global::getTargetFilename());
         } else {
            // We can't backup to target in shell mode, or when writing directly
            // to standard out.
            result = false;
         }
         break;

      case Close:
         Global::requestClose (this->limit);
         result = true;
         break;

      case DelimiterSmart:
         result = Global::setSmartQuote (text.length() > 0 ? text[0] : ':');
         break;

      case Exchange:
         {  // swap last found/last search
            std::string tempStr = Global::getLastSearch();
            Global::setLastSearch (Global::getLastModify());
            Global::setLastModify (tempStr);
         }
         result = true;
         break;

      case Full:
         Global::setMode (Global::Full);
         result = true;
         break;

      case Intermediate:
         result = db.save (Global::getTemporaryFilename());
         break;

      case LimitSet:
         Global::setSearchMax (this->limit);
         result = true;
         break;

      case Monitor:
         Global::setMode (Global::Monitor);
         result = true;
         break;

      case Numbers:
         Global::setShowLineNumbers (!Global::getShowLineNumbers());
         result = true;
         break;

      case Prompt:
         Global::setPromptOn (!Global::getPromptOn());
         result = true;
         break;

      case Quiet:
         Global::setMode (Global::Quiet);
         result = true;
         break;

      case RepeatSet:
         Global::setRepeatMax (this->limit);
         result = true;
         break;

      case SetCursorMark:
         Global::setCursorMark (text.length() > 0 ? text[0] : '^');
         result = true;
         break;

      case TerminalMaxSet:
         Global::setTerminalMax (this->limit);
         result = true;
         break;

      case View:
         Global::show (this->limit, std::cerr);
         result = true;
         break;

      case DefineX:
         if (text.length() > 0) {
            Global::setMacroX (text);
         } else {
            std::string line = Global::getLine (Global::getPromptOn() ? "X? " : NULL);
            Global::setMacroX (line);
         }
         result = true;
         break;

      case DefineY:
         if (text.length() > 0) {
            Global::setMacroY (text);
         } else {
            std::string line = Global::getLine (Global::getPromptOn() ? "Y? " : NULL);
            Global::setMacroY (line);
         }
         result = true;
         break;

      case DefineZ:
         if (text.length() > 0) {
            Global::setMacroZ (text);
         } else {
            std::string line = Global::getLine (Global::getPromptOn() ? "Z? " : NULL);
            Global::setMacroZ (line);
         }
         result = true;
         break;

      default:
         std::cout << " *** " <<  CommandParser::name (this->kind)
                   << " not implemented yet.\n";
         result = false;
   }

   // Adjust status for no fail or invert if needs be.
   //
   result = this->twizzle (result);
   return result;
}

//==============================================================================
// Sequences
//==============================================================================
//
void clearSequence (Sequences& sequence)
{
   for (Sequences::iterator si = sequence.begin ();
        si != sequence.end (); ++si)
   {
      Alternatives alternative = *si;

      for (Alternatives::iterator ai = alternative.begin ();
           ai != alternative.end (); ++ai)
      {
         AbstractCommands* action = *ai;
         delete action;
      }
   }
}

//==============================================================================
// Compound Commands
//==============================================================================
//
CompoundCommands::CompoundCommands (const Sequences sequenceIn,
                                    const int numberIn,
                                    const Modifiers modifierIn) :
   AbstractCommands (numberIn, modifierIn)
{
   this->lastCommand = nullptr;
   this->lastSuccessfullCommand = nullptr;
   this->sequence = sequenceIn;
}

//------------------------------------------------------------------------------
//
CompoundCommands::~CompoundCommands ()
{
   clearSequence (this->sequence);
}

//------------------------------------------------------------------------------
//
AbstractCommands* CompoundCommands::getLastCommand() const
{
   return this->lastCommand;
}

//------------------------------------------------------------------------------
//
AbstractCommands* CompoundCommands::getLastSuccessfullCommand() const
{
   return this->lastSuccessfullCommand;
}

//------------------------------------------------------------------------------
// override
std::string CompoundCommands::image () const
{
   std::string result = "Compound";
   return result;
}

//------------------------------------------------------------------------------
//
bool CompoundCommands::execute (DataBuffer& db)
{
   bool result = true;

   const int useRepeat = this->modifier == AMTAP ? Global::getRepeatMax() : this->number;

   this->lastCommand = nullptr;
   this->lastSuccessfullCommand = nullptr;
   for (int j = 0; j < useRepeat; j++) {

      for (Sequences::iterator si = this->sequence.begin ();
           si != this->sequence.end (); ++si)
      {
         Alternatives alternative = *si;

         result = true;  // hypothosize this alternative cmd seq will succeed.
         for (Alternatives::iterator ai = alternative.begin ();
              ai != alternative.end (); ++ai)
         {
            AbstractCommands* command = *ai;
            result = command->execute (db);
            if (Global::getCloseRequested()) return true;
            if (Global::getInterruptRequest()) return true;

            // Save the last executed basic command.
            //
            CompoundCommands* cc = dynamic_cast <CompoundCommands*> (command);
            this->lastCommand = cc ? cc->getLastCommand() : command;
            if (result) {
               this->lastSuccessfullCommand = cc ? cc->getLastSuccessfullCommand() : command;
            }

            if (!result) break;
         }

         // If this alternative command sequence all ok, then we are done.
         //
         if (result) break;

         // If not, we start the next alternative command sequence if it exists.
      }

      if (!result) break;
   }

   // Adjust status for no fail and invert if needs be.
   //
   result = this->twizzle (result);

   return result;
}

// end
