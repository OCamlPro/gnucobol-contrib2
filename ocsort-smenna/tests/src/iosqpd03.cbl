      *-------------------------------------------------------------------------------*
      * **********************************************************
      *  OCSort Tests
      * **********************************************************
      * Author:    Sauro Menna
      * Date:      20160821
      * License
      *    Copyright 2016 Sauro Menna
      *    GNU Lesser General Public License, LGPL, 3.0 (or greater)
      * Purpose:   Generate COBOL fixed file with COMP fields
      *            For Sumfield
      *            Sort/Merge COBOL Program and OCSort data file
      * **********************************************************
      * option:
      * cobc -x -t ..\listing\%1.lst -I ..\copy -Wall -fbinary-size=1--8 
      *      -fnotrunc -fbinary-byteorder=big-endian -o ..\bin\%1 ..\src\%1.CBL 
      * **********************************************************
      *-------------------------------------------------------------------------------*
	   identification division.
       program-id.  iosqpd03.
       environment division.
       input-output section.
       file-control.
           select masterseqfile assign to  external sqpd03
               organization is sequential
               access mode  is sequential.
       data division.
       file section.
       fd masterseqfile.
       01 masterseqrecord.
          05 msr-02s    pic s9(2) comp-3.
          05 filler     pic x.
          05 msr-02     pic 9(2) comp-3.
          05 filler     pic x.
          05 msr-04s    pic s9(4) comp-3.
          05 filler     pic x.
          05 msr-04     pic 9(4) comp-3.
          05 filler     pic x.
          05 msr-06s    pic s9(6) comp-3.
          05 filler     pic x.
          05 msr-06     pic 9(6) comp-3.
          05 filler     pic x.
          05 msr-08s    pic s9(8) comp-3.
          05 filler     pic x.
          05 msr-08     pic 9(8) comp-3.
          05 filler     pic x.
          05 msr-12s    pic s9(12) comp-3.
          05 filler     pic x.
          05 msr-12     pic 9(12) comp-3.
          05 filler     pic x.
          05 msr-14s    pic s9(14) comp-3.
          05 filler     pic x.
          05 msr-14     pic 9(14) comp-3.
          05 filler     pic x.
          05 msr-16s    pic s9(16) comp-3.
          05 filler     pic x.
          05 msr-16     pic 9(16) comp-3.
          05 filler     pic x.
          05 msr-22s    pic s9(22) comp-3.
          05 filler     pic x.
          05 msr-22     pic 9(22) comp-3.
          05 filler     pic x.
          05 msr-26s    pic s9(26) comp-3.
          05 filler     pic x.
          05 msr-26     pic 9(26) comp-3.
          05 filler     pic x.
          05 msr-28s    pic s9(28) comp-3.
          05 filler     pic x.
          05 msr-28     pic 9(28) comp-3.
          05 filler     pic x.
          05 msr-31s    pic s9(31) comp-3.
          05 filler     pic x.
          05 msr-31     pic 9(31) comp-3.
          05 filler     pic x.

       working-storage section.
       01 recordsize			pic 9999.
	   01 wk-tot.
          05 wrkmsr-03s    pic s9(3) comp-3.
          05 wrkmsr-03     pic 9(3) comp-3.
          05 wrkmsr-09s    pic s9(9) comp-3.
          05 wrkmsr-09     pic 9(9) comp-3.
          05 wrkmsr-18s    pic s9(18) comp-3.
          05 wrkmsr-18     pic 9(18) comp-3.

       procedure division.
       begin.
	      move zero to recordsize
          move recordsize  to wrkmsr-03s
		  move wrkmsr-18s  to recordsize
		
          open output masterseqfile.
	   prdi-00.
	      move all "|"                          to masterseqrecord. 
		  move 22                               to msr-02 
          move 22                               to msr-02s
		  move 4444                             to msr-04 
          move 4444                             to msr-04s
		  move 66666                            to msr-06 
          move 66666                            to msr-06s
		  move 8888888                         to msr-08 
          move 8888888                         to msr-08s
		  move 11212121212                     to msr-12 
          move 11212121212                     to msr-12s
		  move 1414141414141                    to msr-14 
          move 1414141414141                    to msr-14s
		  move 1616161616161616                 to msr-16 
          move 1616161616161616                 to msr-16s
		  move 22222222222222222                to msr-22 
          move 22222222222222222                to msr-22s
  		  move 26262626262626262626262626       to msr-26 
          move 26262626262626262626262626       to msr-26s
  		  move 2828282828282828282828282828     to msr-28 
          move 2828282828282828282828282828     to msr-28s
  		  move 4444444444444444444444444444444  to msr-31 
          move 4444444444444444444444444444444  to msr-31s
    	  write masterseqrecord.
	      move all "|"                          to masterseqrecord. 
		  move -22                              to msr-02s
		  move  22                              to msr-02
		  move -4444                            to msr-04s
		  move  4444                            to msr-04
		  move -66666                           to msr-06s
		  move  66666                           to msr-06
		  move -8888888                        to msr-08s
		  move  8888888                        to msr-08 
		  move -11212121212                    to msr-12s
		  move  11212121212                    to msr-12
		  move -1414141414141                   to msr-14s
		  move  1414141414141                   to msr-14
		  move -1616161616161616                to msr-16s
		  move  1616161616161616                to msr-16
		  move  22222222222222222               to msr-22s
          multiply -1 by msr-22s          
		  move  22222222222222222               to msr-22
		  move  26262626262626262626262626      to msr-26s
          multiply -1 by msr-26s          
		  move  26262626262626262626262626      to msr-26
		  move  2828282828282828282828282828    to msr-28s
          multiply -1 by msr-28s          
		  move  2828282828282828282828282828    to msr-28
		  move  4444444444444444444444444444444 to msr-31s
          multiply -1 by msr-31s          
		  move  4444444444444444444444444444444 to msr-31
    	  write masterseqrecord.
       end-close.             
          close masterseqfile.
       end-proc.
          stop run.
