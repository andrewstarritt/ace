# ace

## General

ACE is a command line text editor.

ACE was originally developed at Marconi Radar Systems Ltd, Chelmsford circa 1980.
It was based on the Edinburgh Compatible Context Editor (ECCE) developed at the
University of Edinburgh during the 1970s by Hamish Dewar.

Originally written in Coral 66 for the GEC OS4000 mini computer, versions were
also created for both Locus 16 and Myriad computers. Circa 1990, an Ada83 version
written for VAX-VMS which was readily ported to Rational R1000, and later to Linux
and Windows using the gnat Ada compiler. The code has since been updated to Ada2005
to take advantage of the unbounded strings and child packages.

A graphical based version witten using Borland Delphi is available for Windows.

Due to the demise of gnatmake in CentOS 8, ACE was transcoded to C++ in 2020.

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
Sun Jun 12 20:11:46 AEST 2022
