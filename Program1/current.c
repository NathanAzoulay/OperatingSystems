/* Nathan Azoulay
 * CS 570, Carroll
 * 8/27/2020
 * Program 1: Lexical Analyzer
 */

#include "getword.h"


int getword(char *w){
    int x; 
    int count = 0;
    x = getchar();
    
    while((x == ' ' || x == '\t')) { 
        x = getchar();
    } // get rid of space
    
    /* Checks for new line or tab */
    if(x == '\n' || x == '\t'){
        *w = '\0';
        return 0;
    }
    
    /* Checks for end of file */
    if(x == EOF){
        *w = '\0';
        return -255;
    }
    
    /* Checks for backslash followed by metacharacter */
    if(x == '\\'){
        x = getchar();
        if(x == '<' || x == '>' || x == '|' || x == '&' ){
            *w = x;
            w++;
            count++;
        }
        x = getchar();
    }
    
    /* Check for metacharacters */
    if(x == '<' || x == '>' || x == '|' || x == '&' ){
        
      if(x == '<'){
            *w = x;
            w++;
            x = getchar();
            if(x == '<'){
                *w = x;
                w++;
                *w = '\0';
                return 2;
            }
            else{
                x = ungetc(x, stdin);
                *w = '\0';
                return 1;
            }
        }
        *w = x;
        w++;
        *w = '\0';
        return 1;
    }
    
    /* Loop for rest of characters */
    while(1){
        x = getchar();
        /* Check if storage space is full */
        if(count == STORAGE-1){
            x = ungetc(x,stdin);
            *w = '\0';
            return count;
        }

        /* Check for end of file or space */
        if(x == EOF || x == ' '){
            *w = '\0';
            return count;
        }

        
        /* Check for backslash followed by metacharacter */
        if(x == '\\'){
            x = getchar();
            if(x == '<' || x == '>' || x == '|' || x == '&' ){
                *w = x;
                w++;
                *w = '\0';
                count++;
            }
            x = getchar();
            if(x == '<'){
                return 22;
            }
        }
        
        /* Check for metacharacter */
        if(x == '<' || x == '>' || x == '|' || x == '&' ){
            x = ungetc(x,stdin);
            *w = '\0';
            return count;
        }

        *w = x;
        w++;
        count++;
        x=getchar();
    }

    /*
    if(x == '$'){
        x = getchar();
        
        while(1){

        if(x == EOF || x == ' ' || x == '\t'){
            
            *w = '\0';
            return -(count);
        }

        if(x == '\n'){
             x = ungetc(x,stdin);
            *w = '\0';
            return -(count);
            
        }  

        if(x == '\\'){
            int tmp = getchar();
            if(tmp == '\n'){
                return count;
            }
        }
        if(count == 0 || *(w-1) != x){
            *w = x;
            w++;
            count++;
        }
        
       // x = getchar(); gets rid of repeated character
        } 
    }
    
    
   

    while(1){

        
        if(x == EOF || x == ' ' || x == '\t'){
            *w = '\0';
            return count;
        }

        if(x == '\n'){
            x = ungetc(x,stdin);
            *w = '\0';
            return count;
        }  

        if(x == '\\'){
            int tmp = getchar();
            if(tmp == '\n'){
                return count;
            }

        }
    
        if(count == 0 || *(w-1) != x){
            if(x == '\\' ){
                
                *w = '\0';
                return count;
            }
            *w = x;
            w++;
            count++;
        }
        
        x = getchar();
        */
    
                

}