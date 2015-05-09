#include <stdlib.h>
#include "error.h"
#include "iloc_generator.h"


/**
 * Generates an ILOC assembly program from an abstract syntax tree.
 */
ILOCProgram* iloc_generator_generate(ASTNode* root)
{
    ILOCProgram* program = malloc(sizeof(ILOCProgram));
    program->first = NULL;
    program->last = NULL;

    return program;
}

/**
 * Generates ILOC assembly code for an AST node.
 */
Error iloc_generator_node(ILOCProgram* program, ASTNode* root)
{
    return E_SUCCESS;
}

/**
 * Writes an ILOC assembly program to file.
 */
void iloc_generator_write(ILOCProgram* program, char* filename)
{}

/**
 * Creates an ILOC assembly instruction.
 */
ILOCInstruction* iloc_instruction_create(ILOCOpcode opcode)
{
    ILOCInstruction* instruction = malloc(sizeof(ILOCInstruction));
    instruction->sources = malloc(sizeof(char*) * 2);
    instruction->targets = malloc(sizeof(char*) * 2);
    instruction->previous = NULL;
    instruction->next = NULL;

    return instruction;
}

/**
 * Adds an ILOC instruction to a program.
 */
Error iloc_add_instruction(ILOCProgram* program, ILOCInstruction* instruction)
{
    if (program->last != NULL) {
        program->last->next = instruction;
        instruction->previous = program->last;
        program->last = instruction;
    } else {
        program->first = instruction;
        program->last = instruction;
    }

    return E_SUCCESS;
}

/**
 * Destroys an ILOC program structure and frees its memory.
 */
Error iloc_program_destroy(ILOCProgram** program)
{
    // make sure pointer isn't null
    if (program == NULL || *program == NULL) {
        return error(E_BAD_POINTER, "Bad ILOC program pointer");
    }

    // free each instruction
    for (ILOCInstruction* i = (*program)->first; i != NULL; i = i->next) {
        if (i->previous != NULL) {
            free(i->previous->sources);
            free(i->previous->targets);
            free(i->previous);
        }
    }
    if ((*program)->last != NULL) {
        free((*program)->last);
    }

    // free the primary structure
    free(*program);
    *program = NULL;
}
