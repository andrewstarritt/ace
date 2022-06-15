/* global.cpp
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

#include "global.h"
#include <iostream>

// all static
//
Global::GetLineFuncPtr Global::getLineFunc = nullptr;

bool Global::interruptRequest = false;
bool Global::executeInProgress = false;

bool Global::closeRequest     = false;
bool Global::abandonRequest   = false;
int Global::exitCode          = 0;

std::string Global::macroX   = "";
std::string Global::macroY   = "";
std::string Global::macroZ   = "";

std::string Global::lastModify   = "";
std::string Global::lastSearch   = "";
std::string Global::lastFilename = "";

std::string Global::cursorMark   = "^";

Global::Modes Global::mode     = Global::Monitor;
bool          Global::promptOn = true;
bool          Global::lineNumbers = false;

int Global::searchMax = 100000;
int Global::repeatMax = 50000;
int Global::terminalMax = 160;

static const std::string modeImages [3] = { "Monitor", "Full", "Quiet" };

//==============================================================================
//
//==============================================================================
//
Global::Global() { }

//------------------------------------------------------------------------------
//
Global::~Global() { }

//------------------------------------------------------------------------------
//
void Global::setGetLineFunction (GetLineFuncPtr getLineFuncIn)
{
   Global::getLineFunc = getLineFuncIn;
}

//------------------------------------------------------------------------------
//
Global::GetLineFuncPtr Global::getGetLineFunc ()
{
   return Global::getLineFunc;
}

//------------------------------------------------------------------------------
//
std::string Global::getLine (const char* prompt)
{
   if (!Global::getLineFunc) {
      std::cerr << "get_line function undefined" << std::endl;
      return "%c\n";
   }

   return (*Global::getLineFunc) (prompt);
}

//------------------------------------------------------------------------------
//
void Global::show (const int detail, std::ostream& stream)
{
   // Include version ??
   // Just do the lot ??

   if (detail >= 0) {
      stream << "Macro X = " << Global::macroX << std::endl;
      stream << "Macro Y = " << Global::macroY << std::endl;
      stream << "Macro Z = " << Global::macroZ << std::endl;
   }

   if (detail >= 1) {
      stream << "Last Search: " << Global::lastSearch   << std::endl;
      stream << "Last Modify: " << Global::lastModify   << std::endl;
      stream << "Last File:   " << Global::lastFilename << std::endl;
   }

   if (detail >= 2) {
      stream << "Monitor Mode: " << modeImages [Global::mode] << std::endl;
      stream << "Line Numbers: " << (Global::lineNumbers ? "On" : "Off") << std::endl;
      stream << "Prompting: "    << (Global::promptOn ? "On" : "Off") << std::endl;
   }

   if (detail >= 3) {
      stream << "Repeat Limit: " << Global::repeatMax << std::endl;
      stream << "Search Limit: " << Global::searchMax << std::endl;
      stream << "Terminal Max: " << Global::terminalMax << std::endl;
   }
}

//------------------------------------------------------------------------------
//
void Global::setExecutionInProgress()
{
   Global::executeInProgress = true;
}

//------------------------------------------------------------------------------
//
void Global::clearExecutionInProgress()
{
   Global::executeInProgress = false;
}

//------------------------------------------------------------------------------
//
void Global::setInterruptRequest ()
{
   if (Global::executeInProgress) {
      std::cerr << "\nSIGINT received - quiting current command sequence.\n";
   }
   Global::interruptRequest = true;
}

//------------------------------------------------------------------------------
//
void Global::clearInterruptRequest ()
{
   Global::interruptRequest = false;
}

//------------------------------------------------------------------------------
//
bool Global::getInterruptRequest ()
{
   return Global::interruptRequest;
}

//------------------------------------------------------------------------------
//
void Global::requestClose (const int exitCodeIn)
{
   Global::closeRequest = true;
   Global::exitCode = exitCodeIn;
}

//------------------------------------------------------------------------------
//
bool Global::getCloseRequested ()
{
   return Global::closeRequest;
}

//------------------------------------------------------------------------------
//
void Global::requestAbandon (const int exitCodeIn)
{
   Global::closeRequest = true;    // implicit
   Global::abandonRequest = true;
   Global::exitCode = exitCodeIn;
}

//------------------------------------------------------------------------------
//
bool Global::getAbandonRequested ()
{
   return Global::abandonRequest;
}

//------------------------------------------------------------------------------
//
void Global::setExitCode (const int exitCodeIn)
{
   Global::exitCode = exitCodeIn;
}

//------------------------------------------------------------------------------
//
int Global::getExitCode ()
{
   return Global::exitCode;
}

//------------------------------------------------------------------------------
//
void Global::setMacroX (const std::string text)
{
   Global::macroX = text;
}

//------------------------------------------------------------------------------
//
std::string Global::getMacroX ()
{
   return Global::macroX;
}

//------------------------------------------------------------------------------
//
void Global::setMacroY (const std::string text)
{
   Global::macroY = text;
}

//------------------------------------------------------------------------------
//
std::string Global::getMacroY ()
{
   return Global::macroY;
}

//------------------------------------------------------------------------------
//
void Global::setMacroZ (const std::string text)
{
   Global::macroZ = text;
}

//------------------------------------------------------------------------------
//
std::string Global::getMacroZ ()
{
   return Global::macroZ;
}

//------------------------------------------------------------------------------
//
void Global::setMode (const Modes modeIn)
{
   Global::mode = modeIn;
}

//------------------------------------------------------------------------------
//
Global::Modes Global::getMode ()
{
   return Global::mode;
}

//------------------------------------------------------------------------------
//
void Global::setShowLineNumbers (const bool showLineNumbersIn)
{
   Global::lineNumbers = showLineNumbersIn;
}

//------------------------------------------------------------------------------
//
bool Global::getShowLineNumbers ()
{
   return Global::lineNumbers;
}

//------------------------------------------------------------------------------
//
void Global::setPromptOn (const bool isOn)
{
   Global::promptOn = isOn;
}

//------------------------------------------------------------------------------
//
bool Global::getPromptOn ()
{
   return Global::promptOn;
}

//------------------------------------------------------------------------------
//
void Global::setSearchMax (const int max)
{
   Global::searchMax = (max >= 1) ? max : 1;
}

//------------------------------------------------------------------------------
//
int Global::getSearchMax ()
{
   return Global::searchMax;
}

//------------------------------------------------------------------------------
//
void Global::setRepeatMax (const int max)
{
   Global::repeatMax = (max >= 1) ? max : 1;
}

//------------------------------------------------------------------------------
//
int Global::getRepeatMax ()
{
   return Global::repeatMax;
}

//------------------------------------------------------------------------------
//
void Global::setCursorMark (const std::string text)
{
   if (text.length() >= 1) {
      Global::cursorMark = text.substr (0, 1);
   } else {
      Global::cursorMark = "^";   // reset default
   }
}

//------------------------------------------------------------------------------
//
std::string Global::getCursorMark ()
{
   return Global::cursorMark;
}

//------------------------------------------------------------------------------
//
void Global::setTerminalMax (const int max)
{
   Global::terminalMax = (max >= 32) ? max : 32;
}

//------------------------------------------------------------------------------
//
int Global::getTerminalMax ()
{
   return Global::terminalMax;
}

//------------------------------------------------------------------------------
//
void Global::setLastSearch (const std::string text)
{
   Global::lastSearch = text;
}

//------------------------------------------------------------------------------
//
std::string Global::getLastSearch ()
{
   return Global::lastSearch;
}

//------------------------------------------------------------------------------
//
void Global::setLastModify (const std::string text)
{
   Global::lastModify = text;
}

//------------------------------------------------------------------------------
//
std::string Global::getLastModify ()
{
   return Global::lastModify;
}

//------------------------------------------------------------------------------
//
void Global::setLastFilename (const std::string filename)
{
   Global::lastFilename = filename;
}

//------------------------------------------------------------------------------
//
std::string Global::getLastFilename ()
{
   return Global::lastFilename;
}

// end
