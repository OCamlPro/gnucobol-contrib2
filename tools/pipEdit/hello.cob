----+-*--1----+----2----+----3----+----4----+----5----+----6----+----7----+----8
         IDENTIFICATION DIVISION.
         PROGRAM-ID.                        Hello.
      *     abc def    ghi
         DATA DIVISION.
         WORKING-STORAGE SECTION.
         01  FIELD                          PIC X(40).
         01  FIELD1                         PIC X(10).
         01  FIELD2                         PIC X(3).
         01  FIELD3                         PIC 9(4).
         01  field4                         PIC 9(4) comp-3.
         01  FIELD                          PIC X(15).
      /
         PROCEDURE DIVISION.
         HST-01.
            DISPLAY 'Hello world!'
            DISPLAY 'Hello world! 1'
            DISPLAY 'Hello world! 2'
            DISPLAY 'Hello world! 3'
            DISPLAY 'Hello world! 4'
            DISPLAY 'Hello world! 5'
            DISPLAY 'Hello world! 6'
            DISPLAY 'Hello world! 7'
            DISPLAY 'Hello world! 8'
            display 'hello world! 8.1'
            DISPLAY 'Hello world! 9'
            DISPLAY 'Hello world! 10'
            MOVE 'abc123'               TO FIELD
            DISPLAY 'Content of field=' FIELD
      D     DISPLAY 'And debugging code!!!'
            NEXT SENTENCE
      *     bullshit
      *     bullshit1
      *     bullshit2
            .
         HST-99.
            STOP RUN
            .
