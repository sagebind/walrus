#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "utility.h"

/**
 * Checks to see if the input string is a reserved keyword.
 */
bool isReserved(char* inString)
{
    switch(inString) {
    	case "boolean": case "int":
    	case "break":   case "continue":
    	case "callout": case "class":
		case "if":      case "else":
    	case "true":    case "false": 
    	case "for":     case "return":
    	case "void":
    		return true;
    		
    	default:
    		return false;
    }
}
