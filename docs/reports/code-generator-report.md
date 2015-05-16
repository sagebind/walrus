# Code Generator Report

## QUESTION 1
*How to build your project from the source code. If you use make or ant, tell me. If not, you must include the full command(s) you used to build your compiler. (Remember, if I can’t figure out how to compile your project, your group gets a Fix & Resubmit grade for the project.)*

In order to build our project from source, ensure that you have a GNU C compiler installed, navigate to the root directory of the project, and execute `make` in the command line. To run the test cases, run `make test`.


## QUESTION 2
*A brief description of how your group divided the work. Summarize each group member’s contribution to the project.*

We didn't put an extensive amount of time into the code generator due to the busyness of the end of the semester. Stephen wrote all the skeleton code, code updates and patches, and the report for this project.


## QUESTION 3
*A list of any clarifications, assumptions, or additions to the problem assigned. The project specifications are fairly broad and leave many of the decisions to you. This is an aspect of real software engineering. If you think major clarifications are necessary, please ask the instructor.*

We were able to determine all the information we needed for the code generator from the project specification, and we don't have any clarifications or assumptions that need to stated.


## QUESTION 4
*An overview of your design, an analysis of design alternatives you considered (if any), and key design decisions. Be sure to document and justify all design decisions you make. Any decision accompanied by a convincing argument will be accepted. If you realize there are flaws or deficiencies in your design late in the implementation process, discuss those flaws and how you would have done things differently. Also include any changes you made to previous parts and why they were necessary.*

Our code generator was to be split up into two parts: the instruction generator, and the register selector. Instruction selection is done recursively by examining each relevant node in a program's AST after parsing. Register selection was to use a bottom-up strategy that examined regions of instructions after they were generated.

Because generation was to be split into two parts, code generation was made into two passes. The first pass iterates over the AST and generates structs that represent ILOC instructions. The ILOC program is stored as a sequence of these ILOC instructions, to reduce memory required for storing an ILOC program in memory before the second pass and make the second pass easier to write, instead of re-parsing ILOC code to modify it.

After the instructions are generated into an ILOC program structure with "virtual" registers used, a second pass would be done over the program for register allocation, replacing virtual registers with real ones.

After both passes, the resulting ILOC representation is turned into actual ILOC code and written to an output file.

This version also includes a fix that should've been in *Project 3*; debug information for parsed Decaf files are now written in a `.dbg` file (in an XML format), instead of pretty-printing to STDOUT. The reason the format was switched to XML is we felt that debug information, while should be human-readable, is more often used by debuggers and IDEs than people, and XML is a common format that anyone can parse.


## QUESTION 5
*A brief description of interesting implementation issues. This should include any non-trivial algorithms, techniques, and data structures. It should also include any insights you discovered during this phase of the project. (External sources of implementation help, e.g., websites or books, should be cited here.)*

Since string manipulation is neither fun, nor efficient in C, ILOC programs are temporarily stored as structs, with both opcodes and operand types stored as integers. An entire ILOC program can be represented using this system; all opcodes we felt we would possibly need are defined, and each instruction can store any ILOC instruction type according to the ILOC spec, as far as we know.

Since registers and addresses are numeric, but labels are strings, the "value" of each instruction operand is an anonymous union of these two types to afford some type safety. The type that should be used is indicated by the `type` field, which also indicates if the operand has been initialized/defined by being set to 0.


## QUESTION 6
*A list of known problems with your project, and as much as you know about the cause.*

- If your project fails a provided test case, but you are unable to fix the problem, describe your understanding of the problem.
- If you discover problems in your project in your own testing that you are unable to fix, but are not exposed by the provided test cases, describe the problem as specifically as possible and as much as you can about its cause. If this causes your project to fail hidden test cases, you may still be able to receive some credit for considering the problem. If this problem is not revealed by the hidden test cases, then you will not be penalized for it.

The code generator is not complete; it may occasionally produce valid ILOC code, but not the code it should. Register allocation is not implemented whatsoever. No test cases should be passing.
