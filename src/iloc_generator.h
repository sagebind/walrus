#ifndef WALRUS_ILOC_GENERATOR_H
#define WALRUS_ILOC_GENERATOR_H

#include <stdio.h>
#include "ast.h"


/**
 * Stores a representation of a single ILOC instruction.
 */
typedef struct ILOCInstruction {
    /**
     * A pointer to the previous instruction.
     */
    struct ILOCInstruction* previous;

    /**
     * A pointer to the next instruction.
     */
    struct ILOCInstruction* next;
} ILOCInstruction;

/**
 * Stores a representation of an ILOC program.
 */
typedef struct {
    /**
     * A pointer to the first instruction in the program.
     */
    ILOCInstruction* first;

    /**
     * A pointer to the last instruction in the program.
     */
    ILOCInstruction* last;
} ILOCProgram;

/**
 * Generates an ILOC assembly program from an abstract syntax tree.
 *
 * @param  root The root node of an abstract syntax tree.
 * @return      A structure representing an ILOC assembly program.
 */
ILOCProgram* iloc_generator_generate(ASTNode* root);

/**
 * Writes an ILOC assembly program to file.
 *
 * @param program An ILOC program to write.
 * @param file    An open file stream to write to.
 */
void iloc_generator_write(ILOCProgram* program, FILE* file);

#endif
