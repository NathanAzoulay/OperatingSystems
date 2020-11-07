/* Nathan Azoulay
 * CS 570, Professor John Carroll
 * 9/9/2020
 * Program 2
 * getword.c
 * This program will handle input appropriately by looking for certain characters with different cases.
 * getword returns the count of our character array. We return -255 when EOF has been reached
 */


#include "getword.h"

int DS_FLAG = 0;

int getword( char *w )
{
    int x; // store getchar()
    int count = 0; // character count
    int BACK,APOS = 0; // This variable indicates whether a backslash has been seen or not                              
    DS_FLAG = 0;

	
    while ( ( x = getchar() ) != EOF ){
 		if(count == 254){ // return when storage is full
			ungetc(x,stdin);
			*w = '\0';
			return count;
		}

		if((x== ' ' || x =='\t') && count == 0){ // get rid of leading spaces
			while(x == ' ')
            	x = getchar();	
        }

		if(x== ' '){ // add null terminator and return count when space encountered AND count != 0
            *w = '\0';
            return count;
        } 

		if(x == '~' && count == 0){ // when tilde is first char we store PATH env into array 
			char *p;
			p = getenv("HOME");
			while(*p  != '\0'){
				*w = *p;
				w++;
				p++;
				count++;
			}
			continue;
		}

		if ( x == '\n' || x == '\t'){		// terminate char array when newline encountered
				if(count == 0){
					*w = '\0';
					return count;
				}	
				*w = '\0'; // Places a null character in array to stop reading word from array
				ungetc(x,stdin);
				if(APOS == 1){ 		//catches for closing quote
   					return -2;
   				}
    				return count; // Returns to p0 program
		}

    	if(x == '$' && count == 0){ // lone dollar sign, command separator
				DS_FLAG = 1;
				x = getchar();
				
		}

    	if( x == '\\' ){ // peak at character after backslash
			x = getchar();
		
		    if( APOS == 0)  
			    BACK = 1; // set \ flag if no apostrophe flag
			else{
				if( x == '\'' ){   // when apos=1, there is a backslash you can insert an apostrophe
					*w++ = x;
					count++;
					x = getchar();
				}
				else{			// add backslash to word
					*w++ = '\\';
					count++;
				}	
			}
		}
		
	
		// allow space between word that contains quote or backslash
		// example echo 'hello' or echo \hello results in hello
    	if( x == ' '){
			if(BACK == 1 || APOS == 1){ // allow space if true
				*w++ = x;
				count++;
			}
			else{
				*w = '\0'; // place null terminator and stop reading word
    			return count; 
			}	
		} 

		// if metacharacter, newline, or apostrophe case
		else if( x == '>' || x == '<'|| x == '|'|| x == '\'' || x == '&' ){ 
			
			
			if (BACK == 1){ // add metacharacter after backslash 
				*w++ = x;
				count++;
			}
			else if( x == '\'' && APOS == 0){ // open quote present, wait to see if closing quote is present
				APOS++;	
			}
			else if( x == '\'' && APOS == 1){ //pass word through if closing apostrophe present
				APOS = 0;
				continue;
			}	
			else if( count != 0 && APOS == 0){ // metacharacter, no backlash
				*w = '\0'; // null terminator to stop reading word
				ungetc(x,stdin);
    			return count; 
			}	
			else if( APOS == 1){ // metacharacters included within '' Ex. echo 'Hello<' prints Hello<
				*w++ = x;
				count++;
			}	
			else{ // add metacharacter to w array
				*w++ = x;
				count++;
				*w = '\0';
				return count;
			}
		}
		// handles regular characters by adding to array and incrementing count
		else{
			*w++ = x; 
			count++;	  
		}
	
		BACK = 0; // reset backslash flag
		
    } // end while loop

   	if(APOS == 1){ // when no closing quote we report back to p2 to print error
   		return -5; 
   	}
    *w = '\0'; // This gets excuted when there is an EOF symbol read so 2nd part of printf in p0 gets a blank character
    if( count != 0){	// return word and count before EOF
   		return count;
   	}
    return -255;  // EOF has been reached	             
} 