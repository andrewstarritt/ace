# ace

## General

ACE is a command line text editor, based on the Edinburgh Compatible Context
Editor (ECCE) developed at the University of Edinburgh during the 1970s by
Hamish Dewar.

[Why ACE](#why)<br>
[Usage](#usage)<br>
[Environment Variables](#env)<br>
[Command Syntax](#syntax)<br>
[Commands](#commands)<br>
[Macros](#macros)<br>
[Release Notes](#notes)<br>
[Historical Information](#history)<br>
[Differences between ACE and ECCE](#ecce)<br>

## <a name = "why"/>Why ACE

Nostalga and because I can; and because I still actually use ACE professionally
now and then.
I do not use it for editing code (I'm too used to modern graphical editors now),
however it is used for ad hoc text processing/manipulation - maybe I should
learn awk.

## <a name = "usage"/>Usage

    ace OPTIONS FROM [TO]
    ace -s, --shell
    ace -h, --help
    ace -l, --license
    ace -v, --version
    ace -w, --warranty

For more detail, run:

    ace --help

_Note:_ When ace edits an existing file, i.e. when no TO parameter, a new file
is __not__ created in that the inode of the file remains unchanged.
This means that ace will __not__ break any file system hard links associated
with the file.


## <a name = "env"/>Environment Variables

### ACE_OPTION

This environment variable provides the default value for the --options option.

### ACE_QUIET

When defined to be '1', 'Y' or 'y', this is the same as using the --quiet option

## <a name = "syntax"/>Command Syntax

The general command syntax is available from ace by running:

    ace --help  syntax


## <a name = "commands"/>Commands

The set of available commands is provided by ace by running:

    ace --help  commands

The list gives both the command as typed (e.g. M, M-, %M) together with the
corresponding full descriptive name (Move, MoveBack, Monitor respectively).
Help for a specific command is also provided by ace, for example:

    ace --help  B

    BreakLine: Break current line (insert \\n) at cursor location.<br>
    Syntax: B [Repeat] [{@|?|\\|~}]<br>
    Failure condition: None.

The command parameter is case insensitive and may be the typed command
recognised by the command parser (B in this example) or the full descriptive
name (BreakLine).

A version of ECCE may be found at http://ecce.sourceforge.net/ together with
other usefull information, much of it relavent to ACE in spirit, if not
in detail.


## <a name = "macros"/>Macros

Macros are defined by %X, %Y and %Z special commands and are invoked by the
X, Y and Z quazi commands.
The macro expansion is a basic text substitution without any real smarts.
For example, if  X is defined to be: MK, then X\* is expanded to MK\* as
opposed to (MK)\*.
There is no implied parentheses.
It is often sensible to define a macro as a compound command: ( .... )

Macros may be defined in terms of other macros.
A check for more than 3 levels of expansion inhibits indefinite macro expansion.

Macros are expanded once by the command line parser rather than at command
execution time.

## <a name = "notes"/>Release Notes

### Version 3

Transcoded to C++, and more.

### 3.1.8

Implemented uncover back (u-) commands.
Also fixed some corener case null string search anomalies.

### 3.1.7

Fixed problem with backward searches.

### 3.1.6

Fixed a bug re inverting fail status to successful (missing break in case
statement).

Started adding Release Notes to this README.md file.

### 3.1.5

Added the %B and %I backup commands.
The former saves work in progress to the target file without closing the edit
session; note it does nothing in shell mode as the output is standard output.
The later saves work in progress to unique temporary file (in the /tmp directory).

### 3.1.4

Rationalised what is in the inbuilt help and what is in the README.md file,
in order to minimise unnecessary duplication.

changed the order of link options to keep Debian (bullseye) happy.

If -o,--option is not defined, look for and use the ACE_OPTION environment
variable instead.

Allow repeated substitutions, i.e. commands for the form  f/.../ s/.../n
Added n- (to match i- and s-) for completeness.

Checked for overflow of repeat and search limit values, i.e. values > 2147483647.

Modified the SIGINT received message - this is now only output during actual
command exection.

A number of messages are now directed to stderr as opposed to stdout.

More help/documentation updates.

### 3.1.3

Allow ACE_QUIET environment variable in lieu of --quiet.

Allow X, Y and Z macros to be interactively defined a la version 2.

Implement set mark (%S), and go with green \*\*END\*\*,  red cursor ^
and yellow line numbers.

More help/documentation updates.
Added an ACE vs. ECCE comparison to the README file.

### 3.1.2

In full monitor mode, stop print if last command was a print.
Fixed bug in B (break) command when at the end of file.
Implemented B-, I- and Q- commands.

Improved command line parseing error messages.

Forward/Reverse code rationalisation.

### 3.1.1

Changes since release 2.

Transcoded to C++.

Added some new backward/reverse commands (inspired by ECCE):

A-, B-, G-, I-, N-, P-, Q-, S-, W-

The substitute (S/S-) commands now allow a repetition number.

U- now uncovers to after the text, or before the text in the backwards direction,
such that the text itself is not deleted.

The Now (N/N-) commands. The date time format of the inserted text has changed
from the in-your-face "SUN 12 JUN 2022 16:04:22" to "Sun Jun 12 16:04:29 2022"

A new special command (%N) toggles on/off displaying line numbers by the
print (P/P-) command.

The text for \*\*END\*\*, ^ (cursor) and line numbers have been colourised.

Some options can now be defined as environment varaiables.
Run  ace --help for more detail.

### Versions 1 and 2

No information available.

## <a name = "history"/>Historical Information

ACE was originally developed at Marconi Radar Systems Ltd, Chelmsford circa 1980.
It was based on the Edinburgh Compatible Context Editor (ECCE) developed at
the University of Edinburgh during the 1970s by Hamish Dewar.

Originally written in Coral 66 for the GEC OS4000 mini computer, versions
were also created for both Locus 16 and Myriad computers.
Circa 1990, an Ada83 version written for VAX-VMS which was readily ported
to Rational R1000, and later to Linux and Windows using the Gnat Ada compiler.
The code was subsequenctly updated to Ada2005 to take advantage of the
unbounded strings and child packages.

A graphical based version witten using Borland Delphi is available for Windows.

Due to the demise of Gnat Ada in CentOS 8, ACE was transcoded to C++ in 2020,
and made available to the wider world (github) in 2022.

##  <a name = "ecce"/>Differences between ACE and ECCE

This section provides a comparison between ACE release 3 and the version
of ECCE from the [http://ecce.sourceforge.net/](http://ecce.sourceforge.net/)
web site, specifically [ecce.c](http://ecce.sourceforge.net/ecce.c).

My 1980s memory of ECCE provided the requirement specification for ACE.
Since then both ACE and apparently ECCE have changed.
While ACE divereged from ECCE, ACE will remain ACE, not brought into line
with ECCE, otherwise I might as well just use ECCE.

#### Last search, modify and filename strings

ACE allows & to represent the last searched for text, such that for example:

    f/sometext/ m4 f&

is equivilent to:

    f/sometext/ m4 f/sometext/

The & character is used to represents the last seached for text, and is set
irrespective of the success or failure of the search.

Delete, Find, Traverse, Uncover and Verify all set/use the same global
last search text.

Likewise & can be used to represent the last modified text, and
Insert and Substitute set/use the same last inserted/modified text.

Connect and Output also allow the use of & and set/use the same filename text.

Just to be clear, there are three separate and distinct last used strings, each
represented by &.

See Quary and %Exchange below to see how the last serched/modified text values
can be modified.

#### ACE Specific Commands

The following additional commands are available in ACE.

A  : Absorbe - get line from secondary input file<br>
C  : Connect - open secondary input file<br>
H  : UpperCase - convert the next *n* character to upper case<br>
N  : Now - insert current date time<br>
O  : Output - create secondary input file<br>
Q  : Quary - capture *n* characters and set as last insert text.<br>
W  : Write - put line to secondary output file<br>

The following back/reverse commands are available:

D- : DeleteBack<br>
F- : FindBack<br>
H- : LowerCase (in forward direction)<br>
K- : KillBack<br>
T- : TraverseBack<br>
U- : UncoverBack<br>
V- : VerifyBack<br>

These turned out very similar to the backwards/reverse command subsequently
discovered in ECCE (great minds think alike), however read the searching
backwards notes below.

For release 3, the following commands have also been added.
These are alligned, where possible, with the functionality discovered in the
lastest version of ECCE.

A- : AbsorbeBack<br>
B- : BreakLineBack<br>
G- : GetBack<br>
I- : InsertBack<br>
J- : JoinBack<br>
N- : NowBack<br>
P- : PrintBack<br>
Q- : QuaryBack<br>
S- : SubstituteBack<br>
W- : WriteBack<br>

#### ACE Special Commands

The following special commands have been added to ACE.

%B : Backup     - save current content of the edit session to the to/target file.<br>
%E : Exchange   - swap last search for text and last inserted text strings.<br>
%I : Intermediate - save current content of the edit session to a temporary file.<br>
%L : LimitSet   - re-define the of number of lines searched for text.<br>
%N : Numbers    - toggle on/off line number inclusion with P/P-.<br>
%P : Prompt     - toggle off/on the '>' command prompt<br>
%R : RepeatSet  - re-define the of number repeats associated with '\*' or '0'.<br>
%S : SetMark    - Modify the cursor character - default is ^.<br>
%T : TerminalMaxSet - set max output length used by the P/P- commands.<br>
%V : View       - display macro values, last search/insert/file strings. <br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
toggle flags, repeat limit, search limit, terminal max.<br>
%X : DefineX    - defines macro X.<br>
%Y : DefineY    - defines macro Y.<br>
%Z : DefineZ    - defines macro Z.<br>


#### Special Commands General

ECCE does not allow regular commands and special commands to be mixed
in the same command line.
ACE does allow commands to be mixed.
Special commands do not fail per se, nor may they be repeated or qualified.
Mixing commands and special commands extends the capability of ACE, e.g.

    v/header/, %c4   # Not the file we are looking for just close (exit code 4)
    # commands for editing header file.
    %c    

#### Searching Backwards

In ACE F- leaves the cursor before the start of the text as "viewed" from
the start of the file, whereas ECCE leaves the cursor before text as "viewed"
with respect to the direction of search, i.e. after the search text.

Likewise, in ACE T- leaves the cursor after the end of the text as "viewed"
from the start of the file, while in ecce leaves the cursor after text as
"viewed" with respect to the direction of search, i.e. before the search text.

    ACE  F-/.../  is equivilent to  ECCE  T-\*/.../
    ACE  T-/.../  is equivilent to  ECCE  F-1/.../


#### Left and Right.

While ECCE interprets L- as R  and  R- as L, ACE does not.

#### Upper/Lower Case

ACE provides commands to convert up upper case (H) and to lower case (H-).
ECCE uses C for case conversion.

#### Qualifiers

ECCE Uses '\\' and '?' to invert the command success and forces a no fail
status repectively.
ACE allows these, however it also allows '~' and '@' for invert and no fail repectively.<br>
[On the GEC 4000 series, both '\\' and '?' were special terminal escape characters]

#### Comments

ACE Allows comments on the command stream introduced by an unquoted
'#' or ';' character.
ECCE uses ';' as a command separator.

#### Shell mode

The -s option allows ACE to be used as a shell filter.

The source file is set to standard input, the target file is set to standard output.
Commands are read from the script file, and all reports are sent to /dev/null

Trival example:

    #!/usr/local/bin/ace -s
    # A filter script to remove trailing spaces
    #

    %r 2147483647   # override default * definition.
    (r*  (lv/ /e)* m)*
    %c

    # end


#### Repeat Last Command

ECCE repeats the last command N times when the sole entry on the command line
is an integer number.
ACE does not provide this functionality, however ACE does provide command
history together with command editing à la bash.

#### End of Input (Ctrl+D)

ACE interprets this as the command &nbsp; %C1.
ECCE creates a temp file (e.g. /tmp/file4VjMhp) and stope with exit code 0.

#### Miscellaneous

There is an implicit P1 executed when ACE starts.

The end of file is denoted by \*\*END\*\* as opposed to \*\*\* End \*\*\*.

%A Abandon vs. Abort. Essentially the same idea.
Abandon allows an exit code to be specified.

The last line has newline (\\n) chacter appended if needs be (just like vi).

#### Size/Implementation
ECCE is 1863 lines,  885 statements, of C code, while<br>
ACE  is 4704 lines, 1540 statements, of C++ code.

Note:<br>
ECCE - Edinburgh Compatible Context Editor<br>
ACE  - ACE Context Editor

<font size="-1">Last updated: Thu Jan 26 17:11:28 AEDT 2023</font>
<br>
