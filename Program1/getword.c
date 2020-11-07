/* Nathan Azoulay
 * CS 570, Professor John Carroll
 * 9/15/2020
 * Program 1: Lexical Analyzer
 */

#include "getword.h"
#include <stdlib.h>


int getword(char *w){
    int x; 
    int count = 0;
    int dollMult = 1; // dollMult = -1 when first character is '$', so we can simply return count * dollaMult 
    x = getchar();
    
    /* Loop(skip char) to get rid of spaces and tabs */
    while((x == ' ' || x == '\t')) { 
        x = getchar();
    } 
    
    /* dollMult is changed to -1 when first char is $ in order to make count negative on later returns */
    if(x == '$'){
        dollMult = -1;
        x = getchar();
    } 

    /* Place a null terminator and return 0 when new line or tab is encountered */
    else if(x == '\n' || x == '\t'){
        *w = '\0';
        return 0;
    }
    /* First character EOF check, place null terminator and return -255 to terminate */
    else if(x == EOF){
        *w = '\0';
        return -255;
    }
    else if(x == '~'){ /* First char is '~' we will print HOME path */
        char* p;
        p = getenv("HOME");
        /* Copy each character from path over to buffer */
        while(*p != '\0'){
            *w = *p;
            w++;
            p++;
            count++;
        }
        x = getchar();
        x= getchar();   
    }
    /* Metacharacter is first character of word, add to buffer */
    else if(x == '<' || x == '>' || x == '|' || x == '&' ){
            *w = x;
            w++;
            x = getchar();
            if(x == '<'){ /* '<<' case */
                *w = x;
                count++;
                w++;
            }
            else{
                x = ungetc(x,stdin);
            } /* unget last char to place null terminator after metacharacter */
            *w = '\0';
            count++;
            return count * dollMult;
    }
    
    /* Loop through rest of the word */
    while(1){
        
        /* Move x back and place null terminator when count is 254 since the 254th spot 
         * will be null terminator followed by a return
         */
        if(count == 254){
            x = ungetc(x,stdin);
            *w = '\0';
            return count * dollMult;
        }

        else if(x == '\\'){ 
            x = getchar();
            /* Treat backslash-newline same as a space */
            if(x == '\n'){
                *w = '\0';
                return count * dollMult;
            }
            else{ /* Treat '\\' as normal character otherwise */
                *w = x;
                w++;
                count++;
                x = getchar();
            } 
        }
        else if(x == EOF || x == ' ' || x == '\t'){ /* EOF, space, tab check */
            *w = '\0';
            return count * dollMult;
        }
        else if(x == '\n'){ /* newline check */
            x = ungetc(x,stdin);
            *w = '\0';
            return count * dollMult;
        } 
        else if(x == '<' || x == '>' || x == '|' || x == '&' ){ /* metacharacter check */
            *w = '\0';
            x = ungetc(x,stdin);
            return count * dollMult;

        }
        /* Place char into w, increment accordingly, and get next character otherwise */
        else{
            *w = x;
            w++;
            count++;
            x = getchar();
        }
        
    }
                

}