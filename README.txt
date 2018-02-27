CONTENTS OF THIS FILE
---------------------
   
 * Introduction
 * Requirements
 * Instruction
 * Author

INTRODUCTION
------------
The whole project contains four files which are README.txt, tokenizer.cpp, tokenizer.h, process.cpp, process.h and main.cpp.
tokenizer.h and tokenizer.cpp define a class used to read test file and random-numbers.
process.cpp, process.h define a class used to store the information of process.
main.cpp contains a bunch of methods and global vairiabls to schedule process.

REQUIREMENTS
------------
To run this application requires the compiler support at least c++11

INSTRUCTION
------------
To run this application please type commands below into command line:
module load gcc-6.2.0
g++ -o test main.cpp tokenizer.cpp process.cpp
./test input.txt
./test --verbose input.txt

AUTHOR     
-----------
Shiyao Lei
sl6569@nyu.edu


