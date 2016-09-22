ID DIVISION.
PROGRAM-ID. GC56DATEPICKERDEMO.
*> ***********************************************************************************
*> GnuCOBOL TT (TUI TOOLS) COLLECTION
*> Purpose:    DEMO OF GC56DATEPICKER
*> Tectonics:  cobc -x GC56DATEPICDEMO.COB (use GnuCOBOL 2.0 or greater)
*> Usage:      GC56DATEPICKERDEMO
*> Parameters: none
*> Author:     Eugenio Di Lorenzo - Italia (DILO)
*> License:    Copyright 2016 E.Di Lorenzo - GNU Lesser General Public License, LGPL, 3.0 (or greater)
*> Version:    1.0 2016.06.15
*> Changelog:  1.0 first release.
*> ***********************************************************************************
DATA DIVISION.
WORKING-STORAGE SECTION.
01 black   constant as 0.
01 blue    constant as 1.
01 green   constant as 2.
01 cyan    constant as 3.
01 red     constant as 4.
01 magenta constant as 5.
01 yellow  constant as 6.  *> or Brown
01 white   constant as 7.
01 wDummy            pic x(01) VALUE     ' '.
01  i    pic 999.
01  j    pic 999.
01  wR   pic 999 value zero.
01  wC   pic 999 value zero.
01 wInt        BINARY-SHORT . *> SIGNED.

copy 'GC01BOX.CPY'.
copy 'GC56DATEPICKER.CPY'.


*> ***********************************************************************************
*>           P R O C E D U R E   D I V I S I O N
*> ***********************************************************************************
PROCEDURE DIVISION.
*> sets in order to detect the PgUp, PgDn, PrtSc(screen print), Esc keys,
set environment 'COB_SCREEN_EXCEPTIONS' TO 'Y'.
set environment 'COB_SCREEN_ESC'        TO 'Y'.

initialize Box-area ALL TO VALUE
set  Box-bco  to white
set  Box-fco  to white
move '002006' to Box-r1c1
move '024076' to Box-r2c2
move 'S'      to Box-style
move "R" to Box-3D
move 'Y' to Box-tit
move z' Date Picker DEMO ' to Box-titDES
call GC01BOX using BOX-AREA
display
 'Use cursor keys: right/left, up/down to browse on dates           ' at 1709 with background-color  Box-Bco foreground-color blue highlight
 'PgDown, Tab, Space bar = next month; PgUp= previous month         ' at 1809 with background-color  Box-Bco foreground-color blue highlight
 '+ = next year;  - = previuos year; Home= jump to current date     ' at 1909 with background-color  Box-Bco foreground-color blue highlight
 'F1/F2= change fore/backcolor: F3= change borders; F4= toggle title' at 2009 with background-color  Box-Bco foreground-color blue highlight
 'F5= toggle shadow; F6= toggle language EN/IT F7= change 3D borders' at 2109 with background-color  Box-Bco foreground-color blue highlight
 'Enter= select the date & return to caller; Escape= exit           ' at 2209 with background-color  Box-Bco foreground-color blue highlight

MOVE 'N' to Dtp-shadow
Move 'S' to Dtp-Style
move 'S' to Dtp-Dim      *> Small Size Calendar

move '004013' to Dtp-r1c1
call GC56DATEPICKER  using DatePicker-Area cancel GC56DATEPICKER

display 'User has selected: '  at 2309 with background-color  Box-Bco foreground-color red highlight
        Dtp-DateSel            at 2328 with background-color  Box-Bco foreground-color red highlight
        ' - With key: '        at 2337 with background-color  Box-Bco foreground-color red highlight
        Dtp-Key                at 2350 with background-color  Box-Bco foreground-color red highlight
accept omitted


move '20190907' to Dtp-DateSel
MOVE 'N' to Dtp-shadow
Move 'N' to Dtp-Style
Move 'N' to Dtp-tit
move x'B4' & ' Calendar '  & x'C3' & x'00' to Dtp-titDes
MOVE 'N' to Dtp-Dim      *> Normal Size Calendar
move '004042' to Dtp-r1c1
call GC56DATEPICKER  using DatePicker-Area cancel GC56DATEPICKER

display 'User has selected: '  at 2309 with background-color  Box-Bco foreground-color red highlight
        Dtp-DateSel            at 2328 with background-color  Box-Bco foreground-color red highlight
        ' - With key: '        at 2337 with background-color  Box-Bco foreground-color red highlight
        Dtp-Key                at 2350 with background-color  Box-Bco foreground-color red highlight
accept omitted

goback.

End Program GC56DATEPICKERDEMO.