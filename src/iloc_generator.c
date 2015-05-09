#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "iloc_generator.h"


/**
 * A map from opcode IDs to opcode strings.
 */
static const char* opcode_strings[] = {
    "add", "sub", "mult", "div", "lshift", "rshift", "load", "load_ai",
    "load_ao", "cload", "cload_ai", "cload_ao", "store", "store_ai", "store_ao",
    "cstore", "cstore_ai", "cstore_ao", "i2i", "c2c", "c2i", "i2c", "comp",
    "cmp_lt", "cmp_le", "cmp_eq", "cmp_ge", "cmp_gt", "cmp_ne", "cbr", "cbr_lt",
    "cbr_le", "cbr_eq", "cbr_ge", "cbr_gt", "cbr_ne", "jumpi", "jump"
};

/**
 * Used to determine the next available virtual register.
 */
static int next_register = 0;


/**
 * Generates an ILOC assembly program from an abstract syntax tree.
 */
ILOCProgram* iloc_generator_generate(ASTNode* root)
{
    ILOCProgram* program = malloc(sizeof(ILOCProgram));
    program->first = NULL;
    program->last = NULL;

    // first, generate the instructions
    iloc_generator_generate_instructions(program, root);

    // then, we need to assign real registers for each instruction

    return program;
}

/**
 * Generates ILOC assembly code for an AST node.
 */
Error iloc_generator_generate_instructions(ILOCProgram* program, ASTNode* node)
{
    // binary operation
    if (node->kind == AST_BINARY_OP) {
        // addition
        if (strcmp(((ASTOperation*)node)->operator, "+") == 0) {
            ILOCInstruction* instr = iloc_instruction_create(ILOC_ADD);
            iloc_add_instruction(program, instr);
        }
    }

    // generate code for each child node
    for (int i = 0; i < node->child_count; ++i) {
        iloc_generator_generate_instructions(program, node->children[i]);
    }

    return E_SUCCESS;
}

/**
 * Writes an ILOC assembly program to file.
 */
void iloc_generator_write(ILOCProgram* program, char* filename)
{
    FILE* stream = fopen(filename, "wb");

    // loop over each instruction sequentially
    for (ILOCInstruction* instr = program->first; instr != NULL; instr = instr->next) {
        // first write the opcode string
        fprintf(stream, "%s", opcode_strings[instr->opcode]);

        // write the source operands
        for (int i = 0; i < 2 && instr->sources[i].type != 0; ++i) {
            // determine the operand type
            if (instr->sources[i].type == ILOC_TYPE_LABEL) {
                fprintf(stream, "%s %s", i > 0 ? "," : "", instr->sources[i].label);
            } else {
                fprintf(stream, "%s %s%d",
                    i > 0 ? "," : "",
                    instr->sources[i].type == ILOC_TYPE_REGISTER ? "r" : "",
                    instr->sources[i].num);
            }
        }

        // write the correct arrow type
        if (instr->opcode == ILOC_CBR
            || instr->opcode == ILOC_CBR_LT
            || instr->opcode == ILOC_CBR_LE
            || instr->opcode == ILOC_CBR_EQ
            || instr->opcode == ILOC_CBR_GE
            || instr->opcode == ILOC_CBR_GT
            || instr->opcode == ILOC_CBR_NE
            || instr->opcode == ILOC_JUMPI
            || instr->opcode == ILOC_JUMP) {
            fprintf(stream, " ->");
        } else {
            fprintf(stream, " =>");
        }

        // write the target operands
        for (int i = 0; i < 2 && instr->targets[i].type != 0; ++i) {
            // determine the operand type
            if (instr->targets[i].type == ILOC_TYPE_LABEL) {
                fprintf(stream, "%s %s", i > 0 ? "," : "", instr->targets[i].label);
            } else {
                fprintf(stream, "%s %s%d",
                    i > 0 ? "," : "",
                    instr->targets[i].type == ILOC_TYPE_REGISTER ? "r" : "",
                    instr->targets[i].num);
            }
        }

        fprintf(stream, "\n");
    }

    fclose(stream);
}

/**
 * Creates an ILOC assembly instruction.
 */
ILOCInstruction* iloc_instruction_create(ILOCOpcode opcode)
{
    // allocate memory for the instruction
    ILOCInstruction* instruction = malloc(sizeof(ILOCInstruction));

    // set the opcode and default values
    instruction->previous = NULL;
    instruction->next = NULL;
    instruction->opcode = opcode;

    // allocate source and target arrays
    instruction->sources = calloc(2, sizeof(ILOCOperand));
    instruction->targets = calloc(2, sizeof(ILOCOperand));

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
