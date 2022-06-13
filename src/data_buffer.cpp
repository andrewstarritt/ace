/* data_buffer.cpp
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

#include "data_buffer.h"
#include "global.h"
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MIN(a, b)          ((a) <= (b) ? (a) : (b))
#define MAX(a, b)          ((a) >= (b) ? (a) : (b))
#define LxIMIT(x,low,high) (MAX(low, MIN(x, high)))

//------------------------------------------------------------------------------
//
DataBuffer::DataBuffer ()
{
   this->lineIter = this->data.begin ();
   this->colNo = 0;
   this->changed = false;

   this->lastSearchType = stVoid;
   this->lastSearchText = "";
}

//------------------------------------------------------------------------------
//
DataBuffer::~DataBuffer ()
{
   this->data.clear();
   this->inputStream.close();
   this->outputStream.close();
}

//------------------------------------------------------------------------------
//
std::string DataBuffer::stdInOut ()
{
   return "-";
}

//------------------------------------------------------------------------------
//
bool DataBuffer::load (const std::string filename)
{
   bool result;

   this->data.clear();

   if (filename == DataBuffer::stdInOut()) {
      // Use standard input to read "file" data
      //
      result = true;
      while (true) {
         std::string line;
         std::getline (std::cin, line);
         if (std::cin.eof ()) break;
         this->data.push_back (line);
      }

   } else {
      std::ifstream src (filename);
      result = src.is_open ();
      if (result) {
         while (true) {
            std::string line;
            std::getline (src, line);
            if (src.eof ()) {
               if (line.size() > 0) {
                  // We have a file where last line has a missing \n
                  // Let's convert to a properly formatted file
                  //
                  this->data.push_back (line);
               }
               break;
            }
            this->data.push_back (line);
         }
         src.close();

      } else {
         std::string message;
         message = "ace: load: " + filename;
         perror (message.c_str());
      }
   }

   // Move to the begining of the file.
   //
   this->lineIter = this->data.begin();
   this->colNo = 0;

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::save (const std::string filename)
{
   const int last = this->data.size();

   bool result;

   // Using a local buffer improves the output speed significantly.
   //
   std::string buffer;

   if (filename == DataBuffer::stdInOut()) {
      // Use standard output to write "file" data
      //
      result = true;
      for (Iterator it = data.begin (); it != data.end (); ++it) {
         std::string line = *it;

         buffer.append (line);
         buffer.append ("\n");   // Linux specific

         if (buffer.length() >= 8192) {
            std::cout << buffer;
            buffer.clear();
         }
      }

      if (buffer.length() > 0) {
         std::cout << buffer;
         buffer = "";
      }

      std::cerr << "Output complete, " << last
                << " lines written to standard output." << std::endl;

   } else {
      std::ofstream dest (filename);
      result = dest.is_open();
      if (result) {

         for (Iterator it = data.begin (); it != data.end (); ++it) {
            std::string line = *it;

            buffer.append (line);
            buffer.append ("\n");   // Linux specific

            if (buffer.length() >= 8192) {
               dest << buffer;
               buffer = "";
            }
         }

         if (buffer.length() > 0) {
            dest << buffer;
            buffer.clear();
         }

         dest.close ();
         std::cerr << "Output complete, " << last
                   << " lines written to: " << filename << std::endl;

      } else {
         std::string message;
         message = "ace: save: " + filename;
         perror (message.c_str());
      }
   }

   return result;
}

//------------------------------------------------------------------------------
//
void DataBuffer::appendLine (const std::string line)
{
   this->data.push_back (line);
}

//------------------------------------------------------------------------------
// insert line just before the current line.
//
void DataBuffer::insertLine (const std::string line)
{
   this->data.insert (this->lineIter, line);
}

//------------------------------------------------------------------------------
// remove the iter line, 'move' to next.
//
void DataBuffer::removeLine (Iterator& iter)
{
   if (iter != this->data.end()) {
      Iterator temp = iter;
      iter++;
      this->data.erase (temp);
   }
}

//------------------------------------------------------------------------------
// replace the current line
//
void DataBuffer::replaceLine (const std::string line)
{
   if (this->lineIter != this->data.end()) {
      *this->lineIter = line;
   }
}

//------------------------------------------------------------------------------
//
std::string DataBuffer::currentLine() const
{
   std::string result;

   if (this->lineIter != this->data.end()) {
      result = *this->lineIter;
   }

   return result;
}

//------------------------------------------------------------------------------
//
int DataBuffer::lineno() const
{
   int result = 1;
   Iterator t = this->lineIter;

   while (t != this->data.begin ()) {
      t--;
      result++;
   }

   return result;
}

//------------------------------------------------------------------------------
//
void DataBuffer::clearChanged ()
{
   this->changed = false;
}

//------------------------------------------------------------------------------
//
void DataBuffer::setChanged ()
{
   this->changed = true;
   this->lastSearchType = stVoid;
   this->lastSearchText = "";
}

//------------------------------------------------------------------------------
//
bool DataBuffer::hasChanged () const
{
   return this->changed;
}

//------------------------------------------------------------------------------
// Used by find, delete, traverse and uncover
//
bool DataBuffer::locate (const int searchLimit, const std::string text,
                         const int skip)
{
   if (this->lineIter == this->data.end ()) {
      return false;
   }

   std::string line = this->currentLine();
   std::string::size_type pos = line.find (text, this->colNo + skip);

   int searchLineCount = 1;
   while ((pos == std::string::npos) && (searchLineCount < searchLimit)) {
      this->lineIter++;
      this->colNo = 0;
      this->setChanged ();
      searchLineCount++;

      if (this->lineIter == this->data.end ()) {
         pos = std::string::npos;
         break;
      }

      line = this->currentLine();
      pos = line.find (text, this->colNo);
   }

   bool result;
   if (pos != std::string::npos) {
      // found
      result = true;
      if (this->colNo != int (pos)) {
         this->colNo = pos;
         this->setChanged ();
      }
   } else {
      // not found
      result = false;
   }

   return result;
}

//------------------------------------------------------------------------------
// Used by findBack, deleteBack, traverseBack and uncoverBack
//
bool DataBuffer::locateBack (const int searchLimit, const std::string text,
                             const int skip)
{
   std::string line = this->currentLine();
   std::string::size_type pos = line.rfind (text, this->colNo - skip);

   int searchLineCount = 1;
   while ((pos == std::string::npos) && (searchLineCount < searchLimit)) {
      if (this->lineIter == this->data.begin ()) {
         pos = std::string::npos;
         break;
      }

      this->lineIter--;
      line = this->currentLine();
      this->colNo = line.length();
      this->setChanged ();
      searchLineCount++;

      pos = line.rfind (text, this->colNo);
   }

   bool result;
   if (pos != std::string::npos) {
      // found
      result = true;
      if (this->colNo != int (pos)) {
         this->colNo = pos;
         this->setChanged ();
      }
   } else {
      // not found
      result = false;
   }

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::absorbeDirection (const Direction direction, const int number)
{
   if (!this->inputStream.is_open()) return false;

   bool result = true;
   for (int j = 0; j < number; j++) {
      std::string line;
      std::getline (this->inputStream, line);
      if (this->inputStream.good()) {
         this->insertLine (line);
         if (direction == Reverse) {
            this->lineIter--;
         }
         this->colNo = 0;
         this->setChanged ();
      } else {
         // end of file.
         //
         this->inputStream.close ();
         result = false;
         break;
      }
   }

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::absorbe (const int number)
{
   return this->absorbeDirection (Forward, number);
}

//------------------------------------------------------------------------------
//
bool DataBuffer::absorbeBack (const int number)
{
   return this->absorbeDirection (Reverse, number);
}

//------------------------------------------------------------------------------
//
bool DataBuffer::breakDirection (const Direction direction, const int number)
{
   bool result = true;

   if (this->lineIter != this->data.end ()) {
      for (int j = 0; j < number; j++) {
         const std::string line = this->currentLine ();
         // Split current line into two parts.
         const std::string part1 = line.substr (0, this->colNo);
         const std::string part2 = line.substr (this->colNo);

         this->replaceLine (part1);
         this->lineIter++;
         this->insertLine (part2);
         if (direction == Forward) {
            this->lineIter--;
            this->colNo = 0;
         } else {
            this->lineIter--;
            this->lineIter--;
            const std::string line2 = this->currentLine ();
            this->colNo = line2.length();
         }
         this->setChanged ();
      }
   } else {
      // We can break the notional **END** line.
      //
      for (int j = 0; j < number; j++) {
         this->appendLine ("");
         // append or inserting at end of buffer "stuffs" up lineIter, so reset.
         //
         this->lineIter = this->data.end ();
         if (direction == Reverse) {
            this->lineIter--;
         }
         this->colNo = 0;
      }
   }

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::breakLine (const int number)
{
   return this->breakDirection (Forward, number);
}

//------------------------------------------------------------------------------
//
bool DataBuffer::breakLineBack (const int number)
{
   return this->breakDirection (Reverse, number);
}

//------------------------------------------------------------------------------
//
bool DataBuffer::connect (const std::string filename)
{
   bool result;

   if (this->inputStream.is_open()) {
      this->inputStream.close();
   }

   if (!filename.empty()) {
      this->inputStream.open (filename);
      result = this->inputStream.is_open();
   } else {
      result = true;  // just closing the file - always successfull.
   }

   return  result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::output (const std::string filename)
{
   bool result;

   if (this->outputStream.is_open()) {
      this->outputStream.close();
   }

   if (!filename.empty()) {
      this->outputStream.open(filename);
      result = this->outputStream.is_open();
   } else {
      result = true;  // just closing the file - always successfull.
   }

   return  result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::deleteText (const int limit, const std::string text, const int number)
{
   bool result = true;

   const int len = text.length();

   for (int j = 0; j < number; j++) {
      result = this->locate (limit, text, 0);
      if (!result) break;

      std::string line = this->currentLine();
      line.erase (this->colNo, len);
      this->replaceLine (line);
      this->setChanged ();
   }

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::deleteBack (const int limit, const std::string text, const int number)
{
   bool result = true;

   const int len = text.length();

   for (int j = 0; j < number; j++) {
      result = this->locateBack (limit, text, 0);
      if (!result) break;

      std::string line = this->currentLine();
      line.erase (this->colNo, len);
      this->replaceLine (line);
      this->setChanged ();
   }

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::erase (const int number)
{
   if (this->lineIter == this->data.end ()) return false;

   std::string line = this->currentLine();
   const int len = line.length();

   int eraseSize = MIN (number, len - this->colNo);
   line.erase (this->colNo, eraseSize);
   this->replaceLine (line);
   this->setChanged ();

   return (eraseSize == number);
}

//------------------------------------------------------------------------------
//
bool DataBuffer::eraseBack (const int number)
{
   if (this->lineIter == this->data.end ()) return false;

   std::string line = this->currentLine();

   int eraseSize = MIN (number, this->colNo);

   this->colNo -= eraseSize;
   line.erase (this->colNo, eraseSize);
   this->replaceLine (line);
   this->setChanged ();

   return (eraseSize == number);
}

//------------------------------------------------------------------------------
//
bool DataBuffer::getDirection (const Direction direction, const int number)
{
   const Global::GetLineFuncPtr getLine = Global::getGetLineFunc ();
   if (!getLine) {
      std::cerr << "get_line function undefined" << std::endl;
      return false;
   }

   bool result = true;
   for (int j = 0; j < number; j++) {
      std::string line = getLine (":");
      if (line == ":") {
         result = false;
         break;
      }

      this->insertLine (line);
      if (direction == Reverse) {
         this->lineIter--;
      }
      this->colNo = 0;
      this->setChanged ();
   }

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::get (const int number)
{
   return this->getDirection (Forward, number);
}

//------------------------------------------------------------------------------
//
bool DataBuffer::getBack (const int number)
{
   return this->getDirection (Reverse, number);
}

//------------------------------------------------------------------------------
//
bool DataBuffer::upperCase (const int number)
{
   if (this->lineIter == this->data.end ()) return false;

   std::string line = this->currentLine();
   const int len = line.length();

   const bool result = this->colNo + number <= len;
   const int size = MIN (number, len - this->colNo);

   for (int j = this->colNo; j < this->colNo + size; j++) {
      char c = line.at(j);
      line.at(j) = toupper(c);
   }

   this->replaceLine (line);
   this->colNo += size;
   this->setChanged ();

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::lowerCase (const int number)
{
   if (this->lineIter == this->data.end ()) return false;

   std::string line = this->currentLine();
   const int len = line.length();

   const bool result = this->colNo + number <= len;
   const int size = MIN (number, len - this->colNo);

   for (int j = this->colNo; j < this->colNo + size; j++) {
      char c = line.at(j);
      line.at(j) = tolower(c);
   }

   this->replaceLine (line);
   this->colNo += size;
   this->setChanged ();

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::insertDirection  (const Direction direction,
                                   const std::string text, const int number)
{
   if (this->lineIter == this->data.end ()) return false;

   if (text.length() == 0) return true;

   std::string line = this->currentLine();
   for (int j = 0; j < number; j++) {
      line.insert(this->colNo, text);
      if (direction == Forward) {
         this->colNo += text.length();
      }
   }

   this->replaceLine (line);
   this->setChanged ();

   return true;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::insert (const std::string text, const int number)
{
   return this->insertDirection (Forward, text, number);
}

//------------------------------------------------------------------------------
//
bool DataBuffer::insertBack (const std::string text, const int number)
{
   return this->insertDirection (Reverse, text, number);
}

//------------------------------------------------------------------------------
//
bool DataBuffer::join (const int number)
{
   bool result = true;
   for (int j = 0; j < number; j++) {

      if (this->lineIter == this->data.end ()) {
         result = false;
         break;
      }

      Iterator nextLine = this->lineIter;
      nextLine++;

      // Unlike ecce, we don't allow last line join with **END** that
      // removes the newline at end of file.
      //
      if (nextLine == this->data.end ()) {
         result = false;
         break;
      }

      const std::string part1 = this->currentLine ();
      const std::string part2 = *nextLine;

      const std::string line = part1 + part2;
      this->replaceLine (line);
      this->removeLine (nextLine);
      this->colNo = part1.length();
      this->setChanged ();
   }

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::joinBack (const int number)
{
   bool result = true;
   for (int j = 0; j < number; j++) {

      if (this->lineIter == this->data.begin () ||
          this->lineIter == this->data.end ()) {
         result = false;
         break;
      }

      Iterator prevLine = this->lineIter;
      prevLine--;

      const std::string part1 = *prevLine;
      const std::string part2 = this->currentLine ();

      const std::string line = part1 + part2;
      this->replaceLine (line);
      this->removeLine (prevLine);
      this->colNo = part1.length();
      this->setChanged ();
   }

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::kill (const int number)
{
   bool result = true;

   for (int j = 0; j < number; j++) {
      if (this->lineIter == this->data.end ()) {
         result = false;
         break;
      }

      this->removeLine (this->lineIter);  // effectively does a lineIter++
      this->colNo = 0;
      this->setChanged ();
   }

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::killBack (const int number)
{
   bool result = true;

   for (int j = 0; j < number; j++) {
      if (this->colNo != 0) {
         this->colNo = 0;
         this->setChanged ();
      }

      if (this->lineIter == this->data.begin ()) {
         result = false;
         break;
      }

      Iterator prevLine = this->lineIter;
      prevLine--;
      this->removeLine (prevLine);
   }

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::left (const int number)
{
   if (this->lineIter == this->data.end ()) return false;

   int deltaCol = MIN (number, this->colNo);
   this->colNo -= deltaCol;
   if (deltaCol) {
      this->setChanged ();
   }
   return (deltaCol == number);
}

//------------------------------------------------------------------------------
//
bool DataBuffer::right (const int number)
{
   if (this->lineIter == this->data.end ()) return false;

   const std::string line = this->currentLine ();
   const int len = line.length();

   int deltaCol = MIN (number, (len - this->colNo));
   this->colNo += deltaCol;
   if (deltaCol) {
      this->setChanged ();
   }
   return (deltaCol == number);
}

//------------------------------------------------------------------------------
//
bool DataBuffer::move (const int number)
{
   bool result = true;

   for (int j = 0; j < number; j++) {
      if (this->lineIter == this->data.end ()) {
         result = false;
         break;
      }
      this->lineIter++;
      this->colNo = 0;
      this->setChanged ();
   }

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::moveBack (const int number)
{
   bool result = true;

   for (int j = 0; j < number; j++) {

      // Note: we always move to the start of the line regardless.
      //
      if (this->colNo != 0) {
         this->colNo = 0;
         this->setChanged ();
      }

      if (this->lineIter == this->data.begin ()) {
         result = false;
         break;
      }

      this->lineIter--;
      this->setChanged ();
   }

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::now (const int number)
{
   time_t now;
   char buffer [32];

   now = time (NULL);
   ctime_r (&now, buffer);  // example: Sun Aug 16 13:22:34 2020

   // Drop trailing \n.
   //
   int n = strnlen (buffer, sizeof (buffer));
   while ((n > 0) && (buffer [n-1] == '\n')) n--;
   buffer [n] = '\0';

   const std::string dateTime = buffer;
   return this->insert (dateTime, number);
}

//------------------------------------------------------------------------------
//
bool DataBuffer::printDirection (const Direction direction, const int number)
{
   bool result = true;

   const int n = this->data.size();
   char format [32];

   if (Global::getShowLineNumbers()) {
      int m;
      if (n < 10) {
         m = 1;
      } else if (n < 100) {
         m = 2;
      } else if (n < 1000) {
         m = 3;
      } else if (n < 10000) {
         m = 4;
      } else if (n < 100000) {
         m = 5;
      } else {
         m = 6;
      }
      snprintf (format, sizeof (format), "\033[33;1m%%%dd\033[00m ", m);
   }

   for (int j = 0; j < number; j++) {
      if (j >= 1) {
         // We only move before the 2nd and subsequenct print.
         //
         if (direction == Forward) {
            result = this->move (1);
         } else {
            result = this->moveBack (1);
         }
         if (!result) break;
      }

      char lnb [32] = "";
      if (Global::getShowLineNumbers()) {
         snprintf (lnb, sizeof (lnb), format, this->lineno());
      }

      if (this->lineIter == this->data.end ()){
         std::cerr << lnb << "**END**" << std::endl;

      } else {
         const std::string line = this->currentLine ();
         if (number >= 2 || this->colNo == 0) {
            std::cerr << lnb << line << std::endl;
         } else {
            const std::string part1 = line.substr (0, this->colNo);
            const std::string part2 = line.substr (this->colNo);
            std::cerr << lnb << part1 << '^' << part2 << std::endl;
         }
      }
      this->clearChanged ();
   }

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::print (const int number)
{
   return this->printDirection (Forward, number);
}

//------------------------------------------------------------------------------
//
bool DataBuffer::printBack (const int number)
{
   return this->printDirection (Reverse, number);
}

//------------------------------------------------------------------------------
//
bool DataBuffer::quary (const int number)
{
   if (this->lineIter == this->data.end ()) return false;

   const std::string line = this->currentLine ();
   const int len = line.length();

   const bool result = this->colNo + number <= len;
   const int size = MIN (number, len - this->colNo);

   Global::setLastModify (line.substr (this->colNo, size));

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::quaryBack (const int number)
{
   if (this->lineIter == this->data.end ()) return false;

   const std::string line = this->currentLine();
   const bool result = this->colNo >= number;
   const int size = MIN (number, this->colNo);

   Global::setLastModify (line.substr (this->colNo - size, size));

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::substitute (const std::string text)
{
   if (this->lineIter == this->data.end ()) return false;

   if (this->lastSearchType == stVoid) return false;

   const std::string line = this->currentLine ();

   // We should not need this MIN check here, but does no harm.
   //
   const std::string::size_type replaceLen = this->lastSearchText.length ();
   const int from = MIN (line.length(), this->colNo + replaceLen);

   const std::string part1 = line.substr (0, this->colNo);
   const std::string part2 = line.substr (from);  // to end of the line

   this->replaceLine (part1 + text + part2);
   this->colNo += text.length();
   this->setChanged ();

   return true;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::substituteBack (const std::string text)
{
   if (this->lineIter == this->data.end ()) return false;

   if (this->lastSearchType == stVoid) return false;

   const std::string line = this->currentLine ();

   // We should not need this MIN check here, but does no harm.
   //
   const std::string::size_type replaceLen = this->lastSearchText.length ();
   const int from = MIN (line.length(), this->colNo + replaceLen);

   const std::string part1 = line.substr (0, this->colNo);
   const std::string part2 = line.substr (from);  // to end of the line

   this->replaceLine (part1 + text + part2);
   /// this->colNo += text.length();
   this->setChanged ();

   return true;
}

//------------------------------------------------------------------------------
// search type commands
//------------------------------------------------------------------------------
//
bool DataBuffer::find (const int limit, const std::string text, const int number)
{
   // Set skip 1 if the last command was a find and
   // we are finding the same text.
   //
   int skip = ((this->lastSearchType == stFind) &&
               (this->lastSearchText == text)) ? 1 : 0;

   bool result = true;
   for (int j = 0; j < number; j++) {
      result = this->locate (limit, text, skip);
      if (!result) break;
      this->lastSearchType = stFind;
      this->lastSearchText = text;
      skip = 1;         // 'last command' is now a find
   }

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::findBack (const int limit, const std::string text, const int number)
{
   bool result = true;
   for (int j = 0; j < number; j++) {
      result = this->locateBack (limit, text, 0);
      if (!result) break;
      this->lastSearchType = stFindBack;
      this->lastSearchText = text;

      // locateBack goes to start/left of text, even when searching backwards.
      // Note: f- and t- have swapped meanings between ace and ecce re
      // location of the cursor after find.
   }

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::traverse (const int limit, const std::string text, const int number)
{
   bool result;

   for (int j = 0; j < number; j++) {
      result = this->locate (limit, text, 0);
      if (!result) break;
      this->lastSearchType = stTraverse;
      this->lastSearchText = text;
      this->colNo += text.length();  // find after
      this->setChanged ();
   }

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::traverseBack (const int limit, const std::string text, const int number)
{
   // Set skip 1 if the last command was a find and
   // we are finding the same text.
   //
   int skip = ((this->lastSearchType == stTraverseBack) &&
               (this->lastSearchText == text)) ? 1 : 0;

   bool result = true;
   for (int j = 0; j < number; j++) {
      result = this->locateBack (limit, text, skip);
      if (!result) break;
      this->lastSearchType = stTraverseBack;
      this->lastSearchText = text;

      // locateBack goes to start/left of text, even when searching backwards.
      // Note: f- and t- have swapped meanings between ace and ecce re
      // location of the cursor after find.
      //
      this->colNo += text.length();
      skip = 1;         // 'last command' is now a stTraverseBack
   }

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::uncover (const int limit, const std::string text, const int number)
{
   // Set skip 1 if the last command was a find and
   // we are finding the same text.
   //
   int skip = ((this->lastSearchType == stUncover) &&
               (this->lastSearchText == text)) ? 1 : 0;

   bool result = true;

   for (int j = 0; j < number; j++) {
      const Iterator lineWhereWeWere = this->lineIter;
      const int colWhereWeWere = this->colNo;

      const std::string part1 = this->currentLine ().substr(0, colWhereWeWere);

      result = this->locate (limit, text, skip);
      if (!result) break;

      if (this->lineIter == lineWhereWeWere) {
         // Found text on the same line.
         //
         std::string line = this->currentLine();
         line.erase (colWhereWeWere, this->colNo - colWhereWeWere);
         this->replaceLine (line);
         this->colNo = colWhereWeWere;
         this->setChanged ();

      } else {
         const std::string part2 = this->currentLine ().substr(this->colNo);

         for (Iterator u = lineWhereWeWere; u != this->lineIter;) {
            this->removeLine (u);  // removeLine does the u++
         }

         const std::string line = part1 + part2;
         this->replaceLine (line);
         this->colNo = colWhereWeWere;
         this->setChanged ();
      }

      this->lastSearchType = stUncover;
      this->lastSearchText = text;
      skip = 1;        // last command is now uncover
   }

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::uncoverBack (const int limit, const std::string text, const int number)
{
   this->lastSearchType = stUncoverBack;
   this->lastSearchText = text;
   return false;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::verify (const std::string text)
{
   if (this->lineIter == this->data.end ()) return false;

   const std::string line = this->currentLine();

   const int tlen = text.length();
   const int amount = line.length() - this->colNo;
   if (tlen > amount) return false;

   const std::string extract = line.substr (this->colNo, tlen);

   bool result = (extract == text);
   if (result) {
      this->lastSearchType = stVerify;
      this->lastSearchText = text;
   }

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::verifyBack (const std::string text)
{
   if (this->lineIter == this->data.end ()) return false;

   const std::string line = this->currentLine();

   const int tlen = text.length();
   const int amount = this->colNo;
   if (tlen > amount) return false;

   const std::string extract = line.substr (this->colNo - tlen, tlen);

   bool result = (extract == text);
   if (result) {
      this->lastSearchType = stVerifyBack;
      this->lastSearchText = text;
   }

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::writeDirection (const Direction direction, const int number)
{
   if (!this->outputStream.is_open()) return false;

   bool result = true;
   for (int j = 0; j < number; j++) {
      if (j >= 1) {
         if (direction == Forward) {
            result = this->move (1);
          } else {
            result = this->moveBack (1);
         }
         if (!result) break;
         this->setChanged ();
      }

      if (this->lineIter != this->data.end ()) {
         const std::string line = this->currentLine ();
         this->outputStream << line << std::endl;
      }
   }

   return result;
}

//------------------------------------------------------------------------------
//
bool DataBuffer::write (const int number)
{
   return this->writeDirection (Forward, number);
}

//------------------------------------------------------------------------------
//
bool DataBuffer::writeBack (const int number)
{
   return this->writeDirection (Reverse, number);
}

// end