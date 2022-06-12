/* ace_main.cpp
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

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "build_datetime.h"
#include "command_parser.h"
#include "commands.h"
#include "data_buffer.h"
#include "global.h"


//------------------------------------------------------------------------------
// Macro to define functions to read and output contents of a resource text file.
//
#define PUT_RESOURCE(function, resource)                                  \
                                                                          \
static void function (std::ostream& stream)                               \
{                                                                         \
   extern char _binary_##resource##_start;                                \
   extern char _binary_##resource##_end;                                  \
                                                                          \
   const char* p = &_binary_##resource##_start;                           \
   while (p < &_binary_##resource##_end) stream << (*p++);                \
}


PUT_RESOURCE (copyright_info, copyright_info_txt)
PUT_RESOURCE (help_general,   help_general_txt)
PUT_RESOURCE (help_info,      help_info_txt)
PUT_RESOURCE (help_ecce,      help_ecce_txt)
PUT_RESOURCE (help_syntax,    help_syntax_txt)
PUT_RESOURCE (help_usage,     help_usage_txt)
PUT_RESOURCE (help_license,   LICENSE)
PUT_RESOURCE (warranty,       warranty_txt)

#undef PUT_RESOURCE

//------------------------------------------------------------------------------
//
static void version (std::ostream& stream)
{
   stream << "Ace Linux Version 3.1.1  Build " << build_datetime() << std::endl;
}

//------------------------------------------------------------------------------
//
static void help (int argc, char** argv)
{
   std::string option = "none";

   // Has the user specified a suplemental help option?
   //
   if (argc >= 1) {
      option = argv[0];
   }

   if ((option == "none") || (option == "")) {
      version (std::cout);
      std::cout << "ace is a command line editor." << std::endl;
      std::cout << std::endl;
      std::cout << "Copyright (C) 1980-2022  Andrew C. Starritt" << std::endl;
      std::cout << std::endl;
      help_usage (std::cout);
      std::cout << std::endl;
      help_general (std::cout);
   }

   else if (option == "info") {
      version (std::cout);
      std::cout << std::endl;
      help_info (std::cout);
   }

   else if (option == "ecce") {
      help_ecce (std::cout);
   }

   else if (option == "syntax") {
      help_syntax (std::cout);
   }

   else if (option == "commands") {
      CommandParser::commandSummary (std::cout);

   } else {
      CommandParser::help (option, std::cout);
   }
}

//------------------------------------------------------------------------------
//
static void signalCatcher (int sig)
{
   switch (sig) {

      case SIGINT:
         std::cout << "\nSIGINT received - quiting current command sequence.\n";
         Global::setInterruptRequest();
         break;

      case SIGTERM:
         std::cout << "\nSIGTERM received - initiating orderly shutdown.\n";
         Global::requestClose(128 + sig);
         break;

      default:
         break;
   }
}


//------------------------------------------------------------------------------
// Used when input is from standard input
//
static std::string getFromStandardInput (const char* prompt)
{
   std::string result;

   char* line = readline (prompt);
   if (line) {
      result = line;

      // If not an empty line, then add to the history.
      // TODO check for line with only white space as well.
      //
      if (line [0] != '\0') {
         add_history (line);
      }

   } else {
      std::cerr << "input terminated" << std::endl;
      Global::requestClose (1);
   }

   return result;
}

//------------------------------------------------------------------------------
// Used when command input is NOT from standard input
//
static std::ifstream commandStream;

static std::string getFromCommandStream (const char* prompt)
{
   std::string result;

   if (commandStream.is_open()) {
      std::getline (commandStream, result);

      if (!commandStream.good ()) {
         // end of file
         std::cerr << "input terminated" << std::endl;
         Global::requestClose (1);
      }

   } else {
      std::cerr << "input terminated" << std::endl;
      Global::requestClose (1);
   }

   return result;
}


//------------------------------------------------------------------------------
//
int main (int argc, char** argv)
{
   // skip program name.
   //
   argc--;
   argv++;

   if (argc < 1) {
      std::cerr << "missing arguments" << std::endl;
      help_usage (std::cerr);
      return 1;
   }

   std::string p1 = argv [0];

   if (p1 == "-h" || p1 == "--help") {
      argc--;
      argv++;
      help (argc, argv);
      return 0;
   }

   if (p1 == "-l" || p1 == "--license") {
      help_license (std::cout);
      return 0;
   }

   if (p1 == "-v" || p1 == "--version") {
      version (std::cout);
      return 0;
   }

   if (p1 == "-w" || p1 == "--warranty") {
      warranty (std::cout);
      return 0;
   }

   enum OptionFlags {
      ofNone    = 0x00,
      ofShell   = 0x01,
      ofCommand = 0x02,
      ofReport  = 0x04,
      ofOption  = 0x08,
      ofBackup  = 0x10,
      ofQuiet   = 0x20,
      ofTheLot  = 0x3F
   };

   // Certain groups of options are mutually exclusive.
   //
   const unsigned meg1 = ofShell;
   const unsigned meg2 = ofTheLot ^ ofShell;

   unsigned optionFlags = ofNone;

   bool shellInterpretor = false;
   bool suppressCopyRight = false;
   std::string command;
   std::string report;
   std::string option;
   std::string backup;
   std::string source;
   std::string target;


// Macro to parse value options
//
#define PARSE_VALUE_OPTION(name, ofFlag) {                               \
   if ((optionFlags & ofFlag) != 0) {                                    \
      std::cerr << "duplicate " #name " option: " << p1 << std::endl;    \
      help_usage (std::cerr);                                            \
      return 1;                                                          \
   }                                                                     \
   /*  */                                                                \
   if (argc < 2) {                                                       \
      std::cerr << "missing " #name " option value" << std::endl;        \
      help_usage (std::cerr);                                            \
      return 1;                                                          \
   }                                                                     \
   /*  */                                                                \
   name = argv [1];                                                      \
   optionFlags |= ofFlag;                                                \
   argc -= 2;                                                            \
   argv += 2;                                                            \
}


   // Parse options/arguments.
   //
   while (argc > 0) {
      p1 = argv [0];
      if (p1[0] != '-') {
         break;  // not an option, start of arguments
      }

      // Check for -- meaning no more options.
      // Usefull when an argument "looks" like an option.
      //
      if (p1 == "--") {
         argc--;
         argv++;
         break;
      }

      if ((p1 == "-c") || (p1 == "--command")) {
         PARSE_VALUE_OPTION (command, ofCommand);
      }

      else if ((p1 == "-r") || (p1 == "--report")) {
         PARSE_VALUE_OPTION (report, ofReport);
      }

      else if ((p1 == "-o") || (p1 == "--option")) {
         PARSE_VALUE_OPTION (option, ofOption);
      }

      else if ((p1 == "-b") || (p1 == "--backup")) {
         PARSE_VALUE_OPTION (backup, ofBackup);
      }

      else if ((p1 == "-s") || (p1 == "--shell")) {
         if ((optionFlags & ofShell) != 0) {
            std::cerr << "duplicate shell option: " << p1 << std::endl;
            help_usage (std::cerr);
            return 1;
         }

         shellInterpretor = true;
         optionFlags |= ofShell;
         argc--;
         argv++;
      }

      else if ((p1 == "-q") || (p1 == "--quiet")) {
         if ((optionFlags & ofQuiet) != 0) {
            std::cerr << "duplicate shell option: " << p1 << std::endl;
            help_usage (std::cerr);
            return 1;
         }

         suppressCopyRight = true;
         optionFlags |= ofQuiet;
         argc--;
         argv++;
      }

      else {
         std::cerr << "unexpected option: " << p1 << std::endl;
         help_usage (std::cerr);
         return 1;
      }
   }

#undef PARSE_VALUE_OPTION

   // Validate option combinations.
   //
   if ( ((optionFlags & meg1) != ofNone) &&
        ((optionFlags & meg2) != ofNone) ) {
      std::cerr << "shell option must be used on own" << std::endl;
      help_usage (std::cerr);
      return 1;
   }

   if (argc < 1) {
      std::cerr << "missing argument" << std::endl;
      help_usage (std::cerr);
      return 1;
   }

   if (shellInterpretor) {
      // Shell commmand mode
      //
      Global::setGetLineFunction (&getFromCommandStream);

      // The first argument provides the commands
      //
      command = argv [0];
      commandStream.open (command);
      bool result = commandStream.is_open ();
      if (!result) {
         std::cerr << "connot open file: " << command << std::endl;
         return 4;
      }

      source = DataBuffer::stdInOut();
      target = DataBuffer::stdInOut();

      // -s implies  -r /dev/null
      //
      report = "/dev/null";
      optionFlags |= ofReport;

   } else {
      // Regular commmand mode
      //
      Global::setGetLineFunction (&getFromStandardInput);

      source = argv [0];
      target = source;
      if (argc >= 2) {
         target = argv [1];
      }

      // TODO: check if source is a directory. ifstream is not so smart here.

      if ((source == target) && (source != DataBuffer::stdInOut())) {
         // Backup source file.
         //
         std::ifstream  src (source,       std::ios::binary);
         std::ofstream  bup (source + "~", std::ios::binary);
         bup << src.rdbuf();
      }
   }

   // Report stream processing.
   //
   if ((optionFlags & ofReport) != ofNone) {
      int fd = creat (report.c_str(), 0644);
      if (fd >= 0) {
         dup2 (fd, STDERR_FILENO); // duplicate fd into standard error
      } else {
         std::cerr << "Cannot open report stream " << report << " : ";
         perror ("");
      }
   }

   // Output the pre-amble.
   //
   if (!Global::getCloseRequested () && !shellInterpretor) {
      version (std::cerr);
      if(!suppressCopyRight) copyright_info (std::cerr);
   }

   // Command backup
   //
   std::ofstream backupStream;
   if ((optionFlags & ofBackup) == ofBackup) {
      backupStream.open(backup);
      if (!backupStream.is_open()) {
         std::cerr << "Warning: Cannot open backup stream " << backup << " : ";
         perror ("");
      }
   }

   // Must call Global::setGetLineFunction before this point.
   //
   DataBuffer db;
   CommandParser parser;
   bool status;

   status = db.load (source);
   if (!status) {
      return 4;
   }

   if (!option.empty()) {
      CompoundCommands* doThis = parser.parse (option);
      if (doThis) {
         bool status = doThis->execute (db);

         if (!status) {
            AbstractCommands* lastCommand = doThis->getLastCommand();
            std::string image = lastCommand ? lastCommand->image() : "None";
            std::cerr << "Command failure: " << image << std::endl;
         }
         delete doThis;
      }
   }

   if (!Global::getCloseRequested () && !shellInterpretor) {
      db.print (1);
   }

   // Moving from option parsing to actual editing.
   //
   signal (SIGTERM, signalCatcher);
   signal (SIGINT,  signalCatcher);

   //---------------------------------------------------------------------------
   // Main processing parse/execute loop
   //---------------------------------------------------------------------------
   //
   while ((shellInterpretor || std::cin) && !Global::getCloseRequested()) {

      std::string line = Global::getLine (Global::getPromptOn ? ">" : NULL);

      if (backupStream.is_open()) {
         backupStream << line << std::endl;
      }

      CompoundCommands* doThis = parser.parse (line);
      if (doThis) {
         db.clearChanged ();   // clear the "dirty" flag.
         Global::clearInterruptRequest();
         bool status = doThis->execute (db);
         if (!status) {
            AbstractCommands* lastCommand = doThis->getLastCommand();
            std::string image = lastCommand ? lastCommand->image() : "None";
            std::cerr << "Command failure: " << image << std::endl;
         }

         switch (Global::getMode()) {
            case Global::Full:
               db.print (1);
               break;

            case Global::Quiet:
               break;

            case Global::Monitor:
               if (db.hasChanged()) {
                  db.print (1);
               }
         }

         delete doThis;
      }
   }

   // Shutting down
   // Save if not abandoned/aborted.
   //
   if (backupStream.is_open()) {
      backupStream.close();
   }

   if (!Global::getAbandonRequested()) {
      status = db.save (target);
      if (!status) {

         char tempName [40];

         // We include uid/pid to enhance mkstemp uniqueness.
         //
         const uid_t u = getuid();
         const pid_t p = getpid();
         snprintf (tempName, sizeof (tempName), "/tmp/ace.tmp_%u_%u_XXXXXX", u, p);

         // We want a name, not open file descriptor (int) value, so we
         // immediately close the file.
         //
         const int tempFd = mkstemp (tempName);
         close (tempFd);

         std::cerr << "Attempting to save contents to an alternative file..." << std::endl;
         status = db.save (tempName);
         if (status) {
            Global::setExitCode (32);
         } else {
            Global::setExitCode (64);
         }
      }
   }

   return Global::getExitCode ();
}

// end

