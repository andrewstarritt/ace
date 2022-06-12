/* global.h
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

#ifndef ACE_GLOBAL_H
#define ACE_GLOBAL_H

#include <string>

class Global
{
public:
   enum Modes {
      Monitor = 0,
      Full,
      Quiet
   };

   typedef std::string (*GetLineFuncPtr) (const char* prompt);

   static void setGetLineFunction (GetLineFuncPtr getLineFunc);
   static GetLineFuncPtr getGetLineFunc ();

   static std::string getLine (const char* prompt);

   static void show (const int detail, std::ostream& stream);

   static void setInterruptRequest ();
   static void clearInterruptRequest ();
   static bool getInterruptRequest ();

   static void requestClose (const int exitCode);
   static bool getCloseRequested ();

   static void requestAbandon (const int exitCode);
   static bool getAbandonRequested ();

   static void setExitCode (const int exitCode);
   static int  getExitCode ();

   static void setMacroX (const std::string text);
   static std::string getMacroX ();

   static void setMacroY (const std::string text);
   static std::string getMacroY ();

   static void setMacroZ (const std::string text);
   static std::string getMacroZ ();

   static void setMode (const Modes mode);
   static Modes getMode ();

   static void setShowLineNumbers (const bool showLineNumbers);
   static bool getShowLineNumbers ();

   static void setPromptOn (const bool isOn);
   static bool getPromptOn ();

   static void setSearchMax (const int max);
   static int getSearchMax ();

   static void setRepeatMax (const int max);
   static int getRepeatMax ();

   static void setTerminalMax (const int max);
   static int getTerminalMax ();

   static void setLastSearch (const std::string text);
   static std::string getLastSearch ();

   static void setLastModify (const std::string text);
   static std::string getLastModify ();

   static void setLastFilename (const std::string filename);
   static std::string getLastFilename ();

private:
   Global();
   ~Global();

   static GetLineFuncPtr getLineFunc;

   static bool interruptRequest;
   static bool closeRequest;
   static bool abandonRequest;
   static int exitCode;

   static std::string macroX;
   static std::string macroY;
   static std::string macroZ;

   static std::string lastModify;
   static std::string lastSearch;
   static std::string lastFilename;

   static Modes mode;
   static bool promptOn;
   static bool lineNumbers;

   static int searchMax;
   static int repeatMax;
   static int terminalMax;
};

#endif // ACE_GLOBAL_H
