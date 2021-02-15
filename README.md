# DSnP_2019fall #
Data Structure and Programming, 2019fall, NTUEE

## HW1 Preparation ##
p1: environment

![](hw1/p1/p1.png)

p2: read in a “.json” file and perform some operations
* Read 
* Print
* Add
* Sum
* Ave
* Max
* Min

## HW2 Command reader ##
design a command reader “cmdReader” for keyboard input and file input.
* Printable character
* Ctrl-a (LINE_BEGIN_KEY) or Home (HOME_KEY)
* Ctrl-e (LINE_END_KEY) or End (END_KEY)
* Backspace (<-) (BACK_SPACE_KEY)
* Delete key (DELETE_KEY)
* Tab key (TAB_KEY)
* Enter  (NEWLINE_KEY)
* Up arrow (ARROW_UP_KEY) or PgUp (PG_UP_KEY)
* Down arrow (ARROW_DOWN_KEY) or PgDn (PG_DN_KEY)

## HW3 Complete UI ##
design a more complete user interface (on top of HW2) for a simple command-line database system (similar to HW1).
* DBAPpend: append a JSON element (key-value pair(s)) to the end of DB
* DBAVerage: compute the average of the DB
* DBCount: report the number of JSON elements in the DB
* DBMAx: report the maximum JSON element
* DBMIn: report the minimum JSON element
* DBPrint: print the JSON element(s) in the DB
* DBRead: read data from .csv file
* DBSOrt: sort the JSON object by key or value
* DBSUm: compute the summation of the DB
* DOfile: execute the commands in the dofile
* HELp: print this help message
* HIStory: print command history
* Quit: quit the execution
