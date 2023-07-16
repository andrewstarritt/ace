/* data_buffer.h
 *
 * This file is part of the ACE command line editor.
 *
 * Copyright (C) 1980-2023  Andrew C. Starritt
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

#ifndef ACE_DATA_BUFFER_H
#define ACE_DATA_BUFFER_H

#include <list>
#include <string>
#include <fstream>

class DataBuffer
{
public:
   enum Direction {
      Forward,
      Reverse       // Aka backwards
   };

   explicit DataBuffer();
   ~DataBuffer();

   // For load/save, a file name of "-" interpreted as stdin or stdout
   // If filename is really "-", then user must use "./-" or similar trick.
   //
   static std::string stdInOut ();

   bool load (const std::string filename);
   bool save (const std::string filename);

   void clearChanged ();
   void setChanged ();

   // Indicates that the location and/or contents has changed since
   // the last print command execuited.
   //
   bool hasChanged () const;

   // Command Parameters
   // number - the number of times to repeat the command. Must be >= 0.
   // text   - search/insert/verify text
   // limit  - limit scope (number of lines) for search like commands.
   //
   // We considered makeing each command take no repeat number and getting the
   // exectution manager to call the required function number times; however
   // it is more efficient to pass the number and some commands are subtley
   // different on repeat, e.g. p2 (print 2) essentially pmp (print move print)
   //
   bool absorbe (const int number);
   bool breakLine (const int number);
   bool connect (const std::string filename);
   bool deleteText (const int limit, const std::string text, const int number);
   bool erase (const int number);
   bool find (const int limit, const std::string text, const int number);
   bool get (const int number);
   bool upperCase (const int number);
   bool insert (const std::string text, const int number);
   bool join (const int number);
   bool kill (const int number);
   bool left (const int number);
   bool move (const int number);
   bool now (const int number);
   bool output (const std::string filename);
   bool print (const int number);
   bool quary (const int number);
   bool right (const int number);
   bool substitute (const std::string text, const int number);
   bool traverse (const int limit, const std::string text, const int number);
   bool uncover (const int limit, const std::string text, const int number);
   bool verify (const std::string text);
   bool write (const int number);

   // Reverse/backwards commands.
   //
   bool absorbeBack (const int number);
   bool breakLineBack (const int number);
   bool deleteBack (const int limit, const std::string text, const int number);
   bool eraseBack (const int number);
   bool findBack (const int limit, const std::string text, const int number);
   bool getBack (const int number);
   bool lowerCase (const int number);
   bool insertBack (const std::string text, const int number);
   bool joinBack (const int number);
   bool killBack (const int number);
   bool moveBack (const int number);
   bool nowBack (const int number);
   bool printBack (const int number);
   bool quaryBack (const int number);
   bool substituteBack (const std::string text, const int number);
   bool traverseBack (const int limit, const std::string text, const int number);
   bool uncoverBack (const int limit, const std::string text, const int number);
   bool verifyBack (const std::string text);
   bool writeBack (const int number);

private:
   typedef std::list<std::string> StringList;
   typedef StringList::iterator   Iterator;

   // These operate on data (StringList).
   // Apart from removeLine, all operate on the current line
   // They do not update colNo.
   // Make inline?
   //
   void appendLine (const std::string line);   // to end of file
   void insertLine (const std::string line);   // before current line
   void removeLine (Iterator& iter);
   void replaceLine (const std::string line);  // replace current line

   // Returns the current line (or empty line).
   //
   std::string currentLine() const;

   // finds the current line number.
   //
   int currentLineNo() const;

   // Basic search functions.
   //
   bool locate     (const int searchLimit, const std::string text, const int skip);
   bool locateBack (const int searchLimit, const std::string text, const int skip);

   // Combined functionality where forward and reverse version of the command
   // are similar.
   //
   bool absorbeDirection    (const Direction direction, const int number);
   bool breakDirection      (const Direction direction, const int number);
   bool getDirection        (const Direction direction, const int number);
   bool insertDirection     (const Direction direction,
                             const std::string text, const int number);
   bool nowDirection        (const Direction direction, const int number);
   bool printDirection      (const Direction direction, const int number);
   bool substituteDirection (const Direction direction,
                             const std::string text, const int number);
   bool writeDirection      (const Direction direction, const int number);

   StringList data;

   Iterator lineIter;    // 0 .. m  where m is current number of lines
   int colNo;            // 0 .. n  where n is line length

   std::ifstream inputStream;    // connect and absorbe
   std::ofstream outputStream;   // output and write

   bool changed;        // indicates some print worthy change has occured.

   // We recall last search type for the substitute command and to allow next
   // instance of the text to be found if/when searching for the same text.
   //
   enum SearchType {
      stVoid,
      stFind,
      stTraverse,
      stUncover,
      stVerify,
      stFindBack,
      stTraverseBack,
      stUncoverBack,
      stVerifyBack
   };

   SearchType  lastSearchType;
   std::string lastSearchText;   // this is not neccesarily Global::setLastSearch
};

#endif // ACE_DATA_BUFFER_H
