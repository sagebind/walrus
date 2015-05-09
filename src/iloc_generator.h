#ifndef WALRUS_ILOC_GENERATOR_H
#define WALRUS_ILOC_GENERATOR_H

#include <stdio.h>
#include "ast.h"
#include "error.h"


/**
 * An enumerated list of possible ILOC opcodes.
 */
typedef enum {
    ILOC_ADD,
    ILOC_SUB,
    ILOC_MULT,
    ILOC_DIV,
    ILOC_LSHIFT,
    ILOC_RSHIFT,
    ILOC_LOAD,
    ILOC_LOAD_AI,
    ILOC_LOAD_AO,
    ILOC_CLOAD,
    ILOC_CLOAD_AI,
    ILOC_CLOAD_AO,
    ILOC_STORE,
    ILOC_STORE_AI,
    ILOC_STORE_AO,
    ILOC_CSTORE,
    ILOC_CSTORE_AI,
    ILOC_CSTORE_AO,
    ILOC_I2I,
    ILOC_C2C,
    ILOC_C2I,
    ILOC_I2C,
    ILOC_COMP,
    ILOC_CMP_LT,
    ILOC_CMP_LE,
    ILOC_CMP_EQ,
    ILOC_CMP_GE,
    ILOC_CMP_GT,
    ILOC_CMP_NE,
    ILOC_CBR,
    ILOC_CBR_LT,
    ILOC_CBR_LE,
    ILOC_CBR_EQ,
    ILOC_CBR_GE,
    ILOC_CBR_GT,
    ILOC_CBR_NE,
    ILOC_JUMPI,
    ILOC_JUMP
} ILOCOpcode;

/**
 * Stores a representation of a single ILOC instruction.
 */
typedef struct ILOCInstruction {
    /**
     * The instruction opcode.
     */
    ILOCOpcode opcode;

    /**
     * The name of a label to the instruction address, if any.
     */
    char* label;

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
 * Generates ILOC assembly code for an AST node.
 *
 * @param  program The ILOC program to generate to
 * @param  root    The node of an abstract syntax tree.
 * @return         A structure representing an ILOC assembly program.
 */
Error iloc_generator_node(ILOCProgram* program, ASTNode* root);

/**
 * Writes an ILOC assembly program to file.
 *
 * @param program An ILOC program to write.
 * @param file    An open file stream to write to.
 */
void iloc_generator_write(ILOCProgram* program, FILE* file);

/**
 * Creates an ILOC assembly instruction.
 *
 * @param  opcode The instruction opcode.
 * @return        A new instruction structure.
 */
ILOCInstruction* iloc_instruction_create(ILOCOpcode opcode);

/**
 * Adds an ILOC instruction to a program.
 *
 * @param  program     The program to add to.
 * @param  instruction The instruction to add.
 * @return             An error code.
 */
Error iloc_add_instruction(ILOCProgram* program, ILOCInstruction* instruction);

/**
 * Destroys an ILOC program structure and frees its memory.
 *
 * @param  program A pointer to the program to destroy.
 * @return         An error code.
 */
Error iloc_program_destroy(ILOCProgram** program);

#endif
