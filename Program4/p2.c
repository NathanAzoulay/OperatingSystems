/* Nathan Azoulay
 * 9/9/2020
 * p2.c
 * The program should replicate a bash shell, executing commands appropriately. We first parse the input and set flags 
 * in order to know what to do with the command. 
 */


#include "p2.h"

extern int DS_FLAG; // $ flag for p4
int numChars;			// used to decide if we are at EOF
char argv[MAXMEM];
char *newargv[MAXITEM];
char *inPtr;
char *outPtr;		// char pointers to in/out files in argv

int inFile, outFile;	// redirection variables
int BG_FLAG = 0; // & flag
int OUT_FLAG = 0; // > flag
int PIPE_FLAG = 0; // | flag
int IN_FLAG = 0; // < flag
int c, j = 0;

int main(){
	(void)signal(SIGTERM,myhandler);
	setpgid(0,0);
	pid_t pid, kidpid;
	DIR *dirp;
	struct stat sb;
	struct dirent *dp;
	int argc;
	
	for(;;){
		printf(":570: ");
		argc = parse();

		if(numChars == -255)  // EOF reached
			break;
		if(numChars == -2){ // Missing closing or beginning apostrophe
			fprintf(stderr,"Unmatched '.\n");
			continue;
		}
		if(argc == 0)		// Reprompt user if empty input
			continue;
		if(newargv[0] == NULL){		//This will be used if the <,> are flagged and no arg
			fprintf(stderr,"Invalid null command.\n");
			continue;
		}

		// "cd" error checking
		if(strcmp(newargv[0],"cd") == 0){
			if(argc > 2)	// too many arguments to cd
				fprintf(stderr,"cd: Too many arguments.\n");
			else if(argc == 1){	// no arguments
				if(chdir(getenv("HOME")) != 0)
					fprintf(stderr,"cd: could not find home directory.\n");
			}
			else{	// cd argument if argc == 2 and check if directory exist
				if(chdir(newargv[1]) != 0){
					fprintf(stderr,"%s: No such file or directory.\n",newargv[1]);
				}
			}
			continue;
		}

		// "ls" code 
		if(strcmp(newargv[0],"ls-l") == 0){
			if(argc > 2){	// too many arguments error
				fprintf(stderr,"ls: Too many arguments.\n");
			}
			if(argc == 1){	// no arguments we print directory were in, if stat doesnt fail
				dirp = opendir(".");
				if(stat(".",&sb) == -1){
					fprintf(stderr,"stat ls failed.\n");
					exit(EXIT_FAILURE);
				}
				while(dirp){ //This will read through the file like in example dir.c
					if((dp = readdir(dirp)) != NULL){
						if(stat(dp->d_name, &sb) == -1)
							fprintf(stderr,"stat ls failed.\n");
						if((sb.st_mode & S_IFMT) == S_IFDIR) // checks if its a directory(S_IFDIR)
							printf("%s\n",dp->d_name);
						else
							printf("%s\n",dp->d_name);
					}
					else{
						closedir(dirp);
						break;
					}
				}
			}	
			else{	// ls has 1 argument, error check first, and then loop through dirp
				if(stat(newargv[1], &sb) == -1){ // if file or director cannot be found
					fprintf(stderr,"%s: No such file or directory.\n",newargv[1]);
					continue;
				}
				if((sb.st_mode & S_IFMT) != S_IFDIR){ // make sure it is a directory
					printf("%s\n",newargv[1]);
					continue;
				}
				if(chdir(newargv[1]) != 0){ // directory cannot be read
					fprintf(stderr,"%s is unreadable\n",newargv[1]);
					continue;
				}
				if((dirp = opendir(newargv[1])) == NULL){ // directory cannot be opened
					printf("%s: failed to open directory\n",newargv[1]);
					continue;
				}	
				while(dirp){ // print items in directory
					if((dp = readdir(dirp)) != NULL){
						if(stat(dp->d_name, &sb) == -1)
							fprintf(stderr,"ls failed.\n");
						if((sb.st_mode & S_IFMT) == S_IFDIR)
							printf("%s\n",dp->d_name);
						else
							printf("%s\n",dp->d_name);
					}
					else{ 
						closedir(dirp);
						break;
					}
				}
			}
			continue;
		}

		// pipe error check
		if(PIPE_FLAG != 0){
			if(PIPE_FLAG > 1){
				fprintf(stderr,"Piping Error: Must contain only one pipe.\n");
				continue;
			}
            else{
                execPipes();
                continue;
            }
		}

		// handle < flag errors
		if(IN_FLAG != 0){
			int flags = O_RDONLY; // read only
			if(IN_FLAG > 2){ // too many inputs
				fprintf(stderr,"Ambiguous input redirect.\n");
				continue;
			}
			if(inPtr+1 == NULL){ // 
				fprintf(stderr,"NULL infile.\n");
				continue;
			}
			if((inFile = open(inPtr,flags)) < 0){
				fprintf(stderr,"Failed to open: %s\n",inPtr);
				continue;
			}
		}

		// Check if > flag contains errors we must consider
		if(OUT_FLAG != 0){
			int flags = O_CREAT | O_EXCL | O_RDWR;
			if(OUT_FLAG > 2){ // flagged more than twice, indicates error
				fprintf(stderr,"Ambiguous input redirect.\n");
				continue;
			}
			if(outPtr == NULL){ // empty outfile pointer needed to redirect
				fprintf(stderr,"Cannot find name for redirect.\n");
				continue;
			}
			// create outfile used to dup2 later while checking if file exists   S_IRWXU
			if((outFile = open(outPtr,flags,0600)) < 0){ // create outfile used to dup2 laterread,write,execute = S_IRWXU
				fprintf(stderr,"%s: Already Exists.\n",outPtr);
				OUT_FLAG = 0;
				continue;
			}
		}

		

		// Fork code, execvp if kid == 0, otherwise terminate program
		fflush(stdout);
		fflush(stderr);
		kidpid = fork();
		if(kidpid < 0){ // fork failed
			printf("Terminating! Cant Fork!");
			exit(1);
		}
		else if(kidpid == 0){ 
			if(BG_FLAG != 0 && IN_FLAG == 0){		// redirect childs stdin to /dev/null/
				int devnull;
				int flags = O_RDONLY;
				if((devnull = open("/dev/null",flags)) < 0){
					fprintf(stderr,"Failed to open /dev/null.\n");
					exit(9);
				}
				if((c = dup2(devnull,STDIN_FILENO))<0){ // create copy of devnull 
					fprintf(stderr,"dup2 did not work.\n");
				}
				close(devnull);
			}
			
			if(IN_FLAG != 0){
				if((c = dup2(inFile,STDIN_FILENO))<0){// create copy of inFile when IN_FLAG is flagged
					fprintf(stderr,"dup2 did not work.\n");
				}
				close(inFile);
			}

			if(OUT_FLAG != 0){  
				if((c = dup2(outFile,STDOUT_FILENO))<0){ // create copy of outFile when OUT_FLAG is flagged
					fprintf(stderr,"dup2 did not work.\n");
				}
				close(outFile);
			}
			
			if((execvp(*newargv, newargv)) < 0){	//execute the command with execvp
				fprintf(stderr,"%s: Command not found.\n",newargv[0]);
				exit(9);
			}
				
		}
		
		// print childs pid after, its stdin has already been redirected to /dev/null above
		if(BG_FLAG !=0){	
			printf("%s [%d]\n", *newargv , kidpid);
			BG_FLAG = 0;
			continue;
		}
		else{	// not background so we will wait until done
			for(;;){
				pid = wait(NULL);
				if(pid == kidpid) 
					break;
			}
		}
	}
	
	// Terminate children still running by signal
	killpg(getpgrp(),SIGTERM); 
	printf("p2 terminated.\n"); 
	exit(0);
}

// Parse method sets global flags as needed, return number of words
int parse(){
	int p = 0;
	int ptr = 0;
	int word_count=0;
	inPtr = '\0'; // points to infile in argv
	outPtr = '\0'; // points to outfile in argv
	IN_FLAG = 0;  // < flag
	OUT_FLAG = 0; // > flag
	BG_FLAG = 0; // & flag
	PIPE_FLAG = 0; // | flag
	j = 0; 

	while((numChars = getword(argv + ptr)) > 0 ){
		if(numChars == -2){		//If there is no closing quote when ex: echo 'hello is entered
			break;
		}
		if(*(argv+ptr) == '&'){	// sets the BG_FLAG "&"
			 if((inPtr == NULL && outPtr == NULL) && (IN_FLAG == 1 || OUT_FLAG == 1)){ 
				break;
			}
			BG_FLAG++;
			break;
		}
		if(*(argv+ptr) == '|'){ // flag |
			PIPE_FLAG++;
			newargv[p++] = NULL;
            j = p;
            continue;
		}		
		else if((*(argv+ptr) == '>' && numChars == 1) || OUT_FLAG == 1){ // sets the OUT_FLAG ">"
			OUT_FLAG++;
			if(OUT_FLAG == 2)
				outPtr = argv+ptr;
		}
		else if((*(argv+ptr) == '<' && numChars == 1) || IN_FLAG == 1){	// sets the IN_FLAG "<"
			IN_FLAG++;
			if(IN_FLAG == 2)
				inPtr = argv +ptr;
		}
		else{
			newargv[p++] =  argv + ptr;
		}
		
		word_count++;
		argv[ptr+ numChars] = '\0'; 
		ptr += numChars+1;
	}
	newargv[p] = NULL; // metacharacter reached
	return word_count;	
}

// handles SIGTERM
void myhandler(){};

// handles pipes
void execPipes(){
    int filedes[2];
    pid_t child, gchild;

    pipe(filedes);

    // First fork
    child = fork();
    if(child == 0){
        dup2(filedes[1],STDOUT_FILENO);
        close(filedes[1]);
        close(filedes[0]);

        // check for INFLAG
        if(IN_FLAG != 0){
            dup2(inFile,STDIN_FILENO);
            close(inFile);
        }
        if((execvp(*newargv,newargv)) < 0){
            fprintf(stderr, "Command not found");
            exit(9);
        }  
    }

    // fork second child
    gchild = fork();
    if(gchild == 0){
        dup2(filedes[1],STDOUT_FILENO);
        close(filedes[1]);
        close(filedes[0]);

        // check for outflag
        if(OUT_FLAG != 0){
            dup2(outFile,STDOUT_FILENO);
            close(outFile);
        }
        if((execvp(newargv[j],newargv+j)) < 0){
            fprintf(stderr, "Command not found");
            exit(9);
        }  
    }

    // only parent reaches here
    close(filedes[0]);
    close(filedes[1]);

    // send STDIN to dev/null
    if(IN_FLAG == 0 && BG_FLAG !=0){
        int devnull;
		int flags = O_RDONLY;
		if((devnull = open("/dev/null",flags)) < 0){
				fprintf(stderr,"Failed to open /dev/null.\n");
				exit(9);
        }

        dup2(devnull,STDIN_FILENO);
        close(devnull);
    }
    if(BG_FLAG !=0){
        printf("%s [%d]\n", *newargv, child);
        BG_FLAG = 0;
    }
    else{
        for(;;){
            pid_t tmp;
            tmp = wait(NULL);
            if(tmp == gchild)
                break;
        }
    }
}
