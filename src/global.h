/* global.h
 *
 * This file is part of the ACE command line editor.
 *
 * SPDX-FileCopyrightText: 1980-2026  Andrew C. Starritt
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Contact details:
 * andrew.starritt@gmail.com
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

   static void setTargetFilename (const std::string filename);
   static std::string getTargetFilename ();

   // More a utility than a global, however ...
   static std::string getTemporaryFilename ();

   static void setExecutionInProgress();
   static void clearExecutionInProgress();

   static void setInterruptRequest ();
   static void clearInterruptRequest ();
   static bool getInterruptRequest ();

   static void requestClose (const int exitCode);
   static bool getCloseRequested ();

   static void requestAbandon (const int exitCode);
   static bool getAbandonRequested ();

   static void setExitCode (const int exitCode);
   static int  getExitCode ();

   static void setMacroX (const std::string& text);
   static std::string getMacroX ();

   static void setMacroY (const std::string& text);
   static std::string getMacroY ();

   static void setMacroZ (const std::string& text);
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

   static void setCursorMark (const char mark);
   static char getCursorMark ();

   static bool setSmartQuote (const char quote);
   static char getSmartQuote ();

   static void setTerminalMax (const int max);
   static int getTerminalMax ();

   static void setLastSearch (const std::string& text);
   static std::string getLastSearch ();

   static void setLastModify (const std::string& text);
   static std::string getLastModify ();

   static void setLastFilename (const std::string& filename);
   static std::string getLastFilename ();

private:
   Global();
   ~Global();

   static GetLineFuncPtr getLineFunc;
   static std::string targetFilename;

   static bool interruptRequest;
   static bool executeInProgress;
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
   static char cursorMark;
   static char smartQuote;

   static int searchMax;
   static int repeatMax;
   static int terminalMax;
};

#endif // ACE_GLOBAL_H
