# ace

Much of the information in this README.md file is available from ace itself.
Do run:

    ace --help


## General

ACE is a command line text editor.

ACE was originally developed at Marconi Radar Systems Ltd, Chelmsford circa 1980.
It was based on the Edinburgh Compatible Context Editor (ECCE) developed at
the University of Edinburgh during the 1970s by Hamish Dewar.

Originally written in Coral 66 for the GEC OS4000 mini computer, versions
were also created for both Locus 16 and Myriad computers.
Circa 1990, an Ada83 version written for VAX-VMS which was readily ported
to Rational R1000, and later to Linux and Windows using the gnat Ada compiler.
The code has since been updated to Ada2005 to take advantage of the
unbounded strings and child packages.

A graphical based version witten using Borland Delphi is available for Windows.

Due to the demise of gnat Ada in CentOS 8, ACE was transcoded to C++ in 2020.

A version of ECCE may be found at http://ecce.sourceforge.net/ecce.c
A comparison between ACE and ECCE can be show using: ace --help ecce

## Release Notes

### Version 3

Changes since release 2.

Added some new backward/reverse commands (somewhat inspired by ECCE):
A-, B-, G-, I-, Q-, P-, S-, W-.

The Now (N) command. The date time format of the inserted text has changed
from the in-your-face "SUN 12 JUN 2022 16:04:22" to "Sun Jun 12 16:04:29 2022"

A new special command (%N) toggles on/off line numbering by the pring (P) command.


## General Syntax

The general format of a command is a single letter, followed zero, one or
more arguments:

    X[-] [repeat] [qualifier]                        -- non text based commands
    X[-] [search_limit] /text/ [repeat] [qualifier]  -- text based commands

White space is ignored (except within strings).

The text argument only applies to string related commands such as F
(for Find text) or I (for Insert text).
String arguments must be quoted.
Any of the following characters may be used as a quote character for a
string argument:<br>
/ &nbsp; "  &nbsp; '  &nbsp; !  &nbsp; .  &nbsp;
\+  &nbsp; ` &nbsp; : &nbsp; = &nbsp; |

_Note:_ The quote character _cannot_ be escaped to allow it to be included
in the text.

The search limit is an optional integer extending or restricting the default
number of lines that are searched for the specified text.

The repeat argument, if allowed, is optional integer.
It specifies the number of times that the command is to be executed.

The qualifier (also optional) is a single character, one of {'\\', '~', '@', '?'}.
The '\\' and '~' qualifier invert the command success status while the
'@' and '?' qualifier force a no fail status.

'\*' or '0' may be used as a combined repeat qualified with no fail and is
equivilent to 50000@, i.e. essentially do many times as possible, but
do not fail.

Special commands are preceeded by a %, and in general do not affect the
the contents of the file, however modify the hehaviour of ace.
%C Closes the edit session.

Compound commands may be formed by grouping other commands  in parentheses
Eg: (mk)* --  this removes every other line from the file (upto line 100000)

Compound commands may be nested.

## Commands

A<br>
Absorbe: Absorbe a line from secondary input file.<br>
Syntax: A [Repeat] [{@|?|\\|~}]<br>
Failure condition: Secondary input file not opened or at end of input file.

A-<br>
AbsorbeBack: Absorbe a line from secondary input file and move back 1 lin.<br>
Syntax: A- [Repeat] [{@|?|\\|~}]<br>
Failure condition: Secondary input file not opened.

B<br>
BreakLine: Break current line (insert \\n) at cursor location.<br>
Syntax: B [Repeat] [{@|?|\\|~}]<br>
Failure condition: None.

B-<br>
BreakLineBack: Break current line (insert \\n) at cursor location.<br>
Syntax: B- [Repeat] [{@|?|\\|~}]<br>
Failure condition: None.

C<br>
Connect: Connect secondary input file specified by /text/.<br>
Syntax: C {/text/,&} [{@|?|\\|~}]<br>
Failure condition: Specified file does not exist or not readable

D<br>
DeleteText: Delete next occurance of specified /text/.<br>
Syntax: D [Limit] {/text/,&} [Repeat] [{@|?|\\|~}]<br>
Failure condition: The text does not occur on the current line or at end of file.

D-<br>
DeleteBack: Delete previous occurance of specified /text/.<br>
Syntax: D- [Limit] {/text/,&} [Repeat] [{@|?|\\|~}]<br>
Failure condition: The text does not occur on the current line.

E<br>
Erase: Erase character to the immediate right of the cursor.<br>
Syntax: E [Repeat] [{@|?|\\|~}]<br>
Failure condition: Cursor is at end of line or at end of file.

E-<br>
EraseBack: Erase character to the immediate left of the cursor.<br>
Syntax: E- [Repeat] [{@|?|\\|~}]<br>
Failure condition: Cursor is at start of line or at end of file.

F<br>
Find: Find next occurance of the specified /text/, cursor located to left of text.<br>
Syntax: F [Limit] {/text/,&} [Repeat] [{@|?|\\|~}]<br>
Failure condition: Specified text does not occur in rest of file.<br>

F-<br>
FindBack: Find previous occurance of the specified /text/, cursor located to left of text.<br>
Syntax: F- [Limit] {/text/,&} [Repeat] [{@|?|\\|~}]<br>
Failure condition: Specified text does not occur in file before current location.

G<br>
Get: Get a line from command input stream.<br>
Syntax: G [Repeat] [{@|?|\\|~}]<br>
Failure condition: Line consists of a single ':'. Usefull for g*

G-<br>
GetBack: Get a line from command input stream and move back 1 line.<br>
Syntax: G- [Repeat] [{@|?|\\|~}]<br>
Failure condition: Line consists of a single ':'. Usefull for g*

H<br>
UpperCase: Convert character to the immediate right of cursor to upper case
and move cursor right one character.<br>
Syntax: H [Repeat] [{@|?|\\|~}]<br>
Failure condition: Cursor is at end of line or at end of file.

H-<br>
LowerCase: Convert character to the immediate right of cursor
to lower case and move cursor right one character.<br>
Syntax: H- [Repeat] [{@|?|\\|~}]<br>
Failure condition: Cursor is at end of line or at end of file.

I<br>
Insert: Insert specified text to the left of the cursor.<br>
Syntax: I {/text/,&} [Repeat] [{@|?|\\|~}]<br>
Failure condition: Cursor is at end of file.

I-<br>
InsertBack: Insert specified text to the right of the cursor.<br>
Syntax: I- {/text/,&} [Repeat] [{@|?|\\|~}]<br>
Failure condition: Cursor is at end of file.

J<br>
Join: Joins the current line with the next line, is effectively D/\\n/.<br>
Syntax: J [Repeat] [{@|?|\\|~}]<br>
Failure condition: Cursor is on last line of file or at end of file.

J-<br>
JoinBack: Joins the previous line with the current line, effectively D-/\\n/.<br>
Syntax: J- [Repeat] [{@|?|\\|~}]<br>
Failure condition: Cursor is on first line of file or at end of file.

K<br>
Kill: Deletes the current line, cursor left at start of next line.<br>
Syntax: K [Repeat] [{@|?|\\|~}]<br>
Failure condition: Cursor is at end of file.

K-<br>
KillBack: Deletes the previous line (K as in Kill), cursor left
at start of the current line.<br>
Syntax: K- [Repeat] [{@|?|\\|~}]<br>
Failure condition: Cursor is on first line of the file.

L<br>
Left: Move cursor left one character.<br>
Syntax: L [Repeat] [{@|?|\\|~}]<br>
Failure condition: Cursor is at start of line or at end of file.

M<br>
Move: Move cursor to the start of the next line.<br>
Syntax: M [Repeat] [{@|?|\\|~}]<br>
Failure condition: Cursor is at end of file.

M-<br>
MoveBack: Move cursor to the start of the previous line.<br>
Syntax: M- [Repeat] [{@|?|\\|~}]<br>
Failure condition: Cursor is at start of file.

N<br>
Now: Insert current date-time to the left of the cursor.<br>
Syntax: N [Repeat] [{@|?|\\|~}]<br>
Failure condition: Cursor is at end of file.

O<br>
Output: Connect secondary output file specified by /text/.<br>
Syntax: O {/text/,&} [{@|?|\\|~}]<br>
Failure condition: Invlaid pathname/cannot create file.

P<br>
Print: Print current line to terminal.<br>
Syntax: P [Repeat] [{@|?|\\|~}]<br>
Failure condition: Cursor is at end of file.

P-<br>
PrintBack: Print current line to terminal.<br>
Syntax: P- [Repeat] [{@|?|\\|~}]<br>
Failure condition: Cursor is at end of file.

Q<br>
Quary: Copies next <repeat> characters to the insert copy buffer.
The cursor is not moved. Note (Q)5 is not the same as Q5.<br>
Syntax: Q [Repeat] [{@|?|\\|~}]<br>
Failure condition: Less than <repeat> characters to the end of the line or
cursor is at end of line or at end of file.

Q-<br>
QuaryBack: Copies next <repeat> characters to the insert copy buffer.
The cursor is not moved. Note (Q)5 is not the same as Q5.<br>
Syntax: Q- [Repeat] [{@|?|\\|~}]<br>
Failure condition: Less than <repeat> characters to the end of the line or
cursor is at end of line or at end of file.

R<br>
Right: Move cursor right one character.<br>
Syntax: R [Repeat] [{@|?|\\|~}]<br>
Failure condition: Cursor is at end of line or at end of file.

S<br>
Substitute: Replace just found text with specified text, cursor
is placed to the left of the replacement text.<br>
Syntax: S {/text/,&} [{@|?|\\|~}]<br>
Failure condition: Previous Find, Uncover, or Verify command failed.

S-<br>
Substitute: Replace just found text with specified text, cursor
is placed to the right of the replacement text.<br>
Syntax: S {/text/,&} [{@|?|\\|~}]<br>
Failure condition: Previous Find, Uncover, or Verify command failed.

T<br>
Traverse: Find after next occurance of specified /text/, cursor located to right of text.<br>
Syntax: T [Limit] {/text/,&} [Repeat] [{@|?|\\|~}]<br>
Failure condition: Specified text does not occur in rest of line.

T-<br>
TraverseBack: Find after previous occurance of specified /text/, cursor located to right of text.<br>
Syntax: T- [Limit] {/text/,&} [Repeat] [{@|?|\\|~}]<br>
Failure condition: Specified text does not occur in file before current location.

U<br>
Uncover: Uncover (remove) characters upto but not including specified /text/.<br>
Syntax: U [Limit] {/text/,&} [Repeat] [{@|?|\\|~}]<br>
Failure condition: The text does not occur on the current line or at end of file.

U-<br>
UncoverBack: Uncover (remove) characters upto and including previous /text/.<br>
Syntax: U- [Limit] {/text/,&} [Repeat] [{@|?|\\|~}]<br>
Failure condition: The text does not occur on the current line.

V<br>
Verify: Compares the text to immediate right of cursor with specified /text/.<br>
Syntax: V {/text/,&} [{@|?|\\|~}]<br>
Failure condition: The text to the immediate right of cursor does not match specified text.

V-<br>
VerifyBack: Verifiy that the text in the file to the immediate
left of the cursor is the same as the specified text.<br>
Syntax: V- {/text/,&} [{@|?|\\|~}]<br>
Failure condition: The text does not match (check is case sensitive).

W<br>
Write: Write current line to secondary output file.<br>
Syntax: W [Repeat] [{@|?|\\|~}]<br>
Failure condition: Secondary output file not open or at end of file.

W-<br>
WriteBack: Write current line to secondary output file.<br>
Syntax: W- [Repeat] [{@|?|\\|~}]<br>
Failure condition: Secondary output file not open or at end of file.


## Special Commands

%A<br>
Abandon: Abandon edit session (exit code 1).<br>
Syntax: %A [Code]<br>
Failure condition: None.

%C<br>
Close: Close edit session (exit code 0).<br>
Syntax: %C [Code]<br>
Failure condition: None.

%E<br>
Exchange: Swap (Exchange) the last found and last inserted strings.<br>
Syntax: %E<br>
Failure condition: None.

%F<br>
Full: Full monitoring mode - current line printed after every command line.<br>
Syntax: %F<br>
Failure condition: None.

%L<br>
LimitSet: Set interpretation of * for search limit - default is 100000.<br>
Syntax: %L [Limit]<br>
Failure condition:

%M<br>
Monitor: Normal monitoring mode - current line printed only if has been modified.<br>
Syntax: %M<br>
Failure condition: None.

%N<br>
line Numbers: Toggle on/off displaying line numbers.<br>
Syntax: %N<br>
Failure condition: None.

%P<br>
Prompt: Toggle off/on the command prompts (>, :, X?, Y?, and/or Z?).
Syntax: %P<br>
Failure condition: None.

%Q<br>
Quiet: Quiet monitoring mode - current line never printed after command line.<br>
Syntax: %Q<br>
Failure condition: None.

%R<br>
RepeatSet: Set the repitition limit used for '\*' and '0' - default is 50000.<br>
Syntax: %R [Limit]<br>
Failure condition: None.

%S<br>
SetMark: Set the cursor indication character to first character of the specified
string.<br>
Syntax: %S {/text/,&}<br>
Failure condition: None.

%T<br>
TerminalMaxSet: Set the command line terminal width (min is 32) - default is 160.<br>
Syntax: %T [Limit]<br>
Failure condition: None.

%V<br>
View: View current settings - increase value for more detail.<br>
Syntax: %V [Number]<br>
Failure condition: None.

%X<br>
DefineX: Define the X macro - if /text/ is empty, the user is prompted.<br>
Syntax: %X {/text/,&}<br>
Failure condition: None.

%Y<br>
DefineY: Define the Y macro - if /text/ is empty, the user is prompted.<br>
Syntax: %Y {/text/,&}<br>
Failure condition: None.

%Z<br>
DefineZ: Define the Z macro - if /text/ is empty, the user is prompted.<br>
Syntax: %Z {/text/,&}<br>
Failure condition: None.

## Macros

The macro defined by %X, %Y and %Z may be invoked by the X, Y and Z quazi commands.
The macro expansion is a basic text substitution without any real smarts.
For example, if  X is defined to be: MK, then X\* is expanded to MK\* as
opposed to (MK)\*.
There is no implied parentheses.
It is often sensible to define a macro as a compound command: ( .... )

Macros may be defined in terms of other macros.
A check for more than 3 levels of expansion inhibits indefinite macro expansion.

Macros are expanded by the command line parser rather than at command
exectiution time.


## Differences between ACE and ECCE

My# 1980 memory of ECCE provided the requirement specification for ACE.

#### Last search, find and filename

ACE was modified to allow & to represent the last search for text, such
that, for example:

    f/sometext/ m4 v/sometext/

is equivilent to:

    f/sometext/ m4 v&

#### New Commands

Delete, Find, Traverse, Uncover and Verify all set/use the same global
last search text. Likewise Insert and Substitute set/use the same last
inserted/modified text and Connect and Output set/use the same filename
text. See Quary and %Exchange below how these can be modified.

The following commands were added to ACE.

A  : Absorbe - get line from secondary input file<br>
C  : Connect - open secondary input file<br>
H  : UpperCase - convert to upper case<br>
N  : Now - insert current date time<br>
O  : Output - create secondary input file<br>
Q  : Quary - capture n character as last insert text.<br>
W  : Write - put line to secondary output file<br>

The following back/reverse commands were added:

D- : DeleteBack<br>
F- : FindBack<br>
H- : LowerCase (in forward direction)<br>
K- : KillBack<br>
T- : TraverseBack<br>
U- : UncoverBack<br>
V- : VerifyBack<br>

These turned out very similar to the backwards command subsequently
re-discovered in ECCE (great minds think alike), however read the
searching backwards notes below.

For release 3, the following commands have also been added. These are
alligned with the functionality re-discovered in ECCE.

A- : AbsorbeBack<br>
B- : BreakLineBack<br>
G- : GetBack<br>
I- : InsertBack<br>
J- : JoinBack<br>
P- : PrintBack<br>
Q- : QuaryBack<br>
S- : SubstituteBack<br>
W- : WriteBack<br>

#### New Special Commands

The following special commands have been added to ACE.

%E : Exchange   - swap last found text and last inserted text strings.<br>
%L : LimitSet   - re-define the of number of lines searched for text.<br>
%N : Numbers    - toggle on/off line number includsion with P/P-.<br>
%P : Prompt     - toggle off/on the '>' command prompt<br>
%R : RepeatSet  - re-define the of number repeats associated with '*' or '0'.<br>
%S : SetMark    - Modify the cursor character - default is ^.<br>
%T : TerminalMaxSet - set max output length used by the P/P- commands.<br>
%V : View       - display macro values, last search/insert/file strings. <br>
                  toggle flags, repeat limit, search limit, terminal max.<br>
%X : DefineX    - defines macro X.<br>
%Y : DefineY    - defines macro Y.<br>
%Z : DefineZ    - defines macro Z.<br>


#### Special Commands General

ECCE does not allow regular commands and special commands to be mixed
in the same command line. ACE does allow commands to be mixed. Special
commands do not fail per se, nor may they be repeated or qualified.

#### Searching Backwards

In ACE F- leaves the cursor before the start of the text as "viewed" from
the start of the file, while in ECCE leaves the cursor before text as "viewed"
with respect to the direction of search, i.e. after the search text.

Likewise, in ACE T- leaves the cursor after the end of the text as "viewed"
from the start of the file, while in ecce leaves the cursor after text as
"viewed" with respect to the direction of search, i.e. before the search text.

    ACE  F-/.../  is equivilent to  ECCE  T-*/.../
    ACE  T-/.../  is equivilent to  ECCE  F-1/.../


#### Left and Right.

While ECCE interprets L- as R  and interprets R- as L,  ACE does not.

#### Upper/Lower Case

ACE provides commands to convert up upper case (H) and to lower case (H-).
ECCE uses C for upper case, and C- for upper case backwards.

#### Qualifiers

ECCE Uses '\' and '?' to invert the command success and forces a no fail
status repectively. ACE allows these, however it also allows '~' and '@'
for invert and no fail repectively.<br>
[On the GEC 4000 series both '\' and '?' where special characters.]

#### Comments

ACE Allows comments on the commands stream introduced by an unquoted
'#' or ';'

#### Repeat Last Command
ECCE repeats the last command N times when the sole entry on the command line
is an integer number. ACE does not, however ACE does provide command history
and command editing � la bash.

#### Size/Implementation
ECCE is 1863 lines,  885 statements, of C code, while
ACE  is 4307 lines, 1424 statements, of C++ code.

Note:
ECCE - Edinburgh Compatible Context Editor
ACE  - ACE Context Editor

<font size="-1">Last updated: Mon Jun 13 17:17:04 AEST 2022</font>
<br>
