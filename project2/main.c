//
//  main.c
//  project
//
//  Created by Sujan Shrestha on 9/19/17.
//  Copyright © 2017 Sujan Shrestha. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

char * getPath(char ** envp);
char ** parse(char * string, char delimeter);

char *fname;
char *fname2;
int runAgain = -1;
bool file1exists;
bool file2exists;

char* command;

//Method for creating new process
void runCommand(char * arr, char ** array)
{
    
    pid_t pid =  fork();
    
    //Child  process goes here
    if(pid == 0)
    {
        
        //printf("Child process started!\n\n");
        //Here we execute
        execv(arr, array);
        exit(0);
        
    }
    else if(pid<0)
    {
        //When fork() returns an error
        printf("Error spwanning the child process!\n");
        exit(-1);
    }
    else
    {
        //Parent process goes here
        // printf(" Parent pid %d\n\n", pid);
        wait(&pid);
    }
}

void runpipeCommand(char * arr, char ** array, char * arr2, char ** array2)
{
    int thePipe[2];
    pipe(thePipe);
    pid_t pid = fork();
    if(pid == 0) // child Process
    {
        close(STDOUT_FILENO);
        close(thePipe[0]);
        dup2(thePipe[1], STDOUT_FILENO);
        execv(arr, array);
    }
    else if( pid <0)
    {
        //When fork() returns an error
        printf("Error spwanning the child process!\n");
        exit(-1);
    }
    else //Parent Process
    {
        wait(&pid);
        close(STDIN_FILENO);
        close(thePipe[1]);
        dup2(thePipe[0], STDIN_FILENO);
        execv(arr2,array2);
    }
    close(thePipe[0]);
    close(thePipe[1]);
    exit(0);
}
/*This method calls the 2 child process)*/
void startProcess(char * arr, char ** array, char * arr2, char ** array2)
{
    
    pid_t pid =  fork();
    
    //Child  process goes here
    if(pid == 0)
    {
        //printf("Child process started!\n\n");
        //Here we call the runpipeCommand Method
        runpipeCommand( arr, array,arr2, array2);
        exit(0);
    }
    else if(pid<0)
    {
        //When fork() returns an error
        printf("Error spwanning the child process!\n");
        exit(-1);
    }
    else
    {
        //Parent process goes here
        //printf(" Parent pid %d\n\n", pid);
        wait(&pid);
    }
}


//Method to check if File exists

void  fileCheck (char ** arrayOfPaths, char ** commandList ,char ** secondCommandList)
{
    if(arrayOfPaths != NULL)
    {
        if(secondCommandList == NULL)
        {
            for( int i= 0; arrayOfPaths[i] != NULL; i++)
            {
                char *arr = arrayOfPaths[i];
                int prefixLen = 0;
                prefixLen = ((int)strlen(arr));
                char* pLocal = NULL;
                pLocal = malloc(sizeof(char) * prefixLen + 2 + (int)strlen(commandList[0]));
                pLocal[0] = 0;
                //printf("plocal %s\n", pLocal);
                strncat(pLocal, &(arr[0]), prefixLen);
                strncat(pLocal, "//",1);
                strcat(pLocal, commandList[0]);
                printf("Checking: %s\n", pLocal);
                //Checking to see if file exists
                if( access( pLocal, F_OK ) != -1 ) {
                    // file exists
                    printf("Found!\n");
                    printf("String = %s\n", commandList[0]);
                    runCommand(pLocal, commandList);
                    break;
                }
                else
                {
                    free(pLocal);
                }
            }
        }
        else{
            fname= NULL;
            fname2 = NULL;
            file1exists = false;
            file2exists = false;
            for( int i= 0; arrayOfPaths[i] != NULL; i++)
            {
                char *arr = arrayOfPaths[i];
                int prefixLen = 0;
                prefixLen = ((int)strlen(arr));
                char* pLocal = NULL;
                pLocal = malloc(sizeof(char) * prefixLen + 2 + (int)strlen(commandList[0]));
                pLocal[0] = 0;
                //printf("plocal %s\n", pLocal);
                strncat(pLocal, &(arr[0]), prefixLen);
                strncat(pLocal, "//",1);
                strcat(pLocal, commandList[0]);
                printf("Checking: %s\n", pLocal);
                //Checking to see if file exists
                if( access( pLocal, F_OK ) != -1 ) {
                    // file exists
                    printf("Found!\n");
                    printf("String = %s\n", commandList[0]);
                    file1exists = true;
                    fname = malloc(sizeof(char)* strlen(pLocal));
                    fname[0] = 0;
                    strcat(fname, pLocal);
                    break;
                }
                else
                {
                    free(pLocal);
                }
            }
            for( int i= 0; arrayOfPaths[i] != NULL; i++)
            {
                char *arr = arrayOfPaths[i];
                int prefixLen = 0;
                prefixLen = ((int)strlen(arr));
                char* pLocal = NULL;
                pLocal = malloc(sizeof(char) * prefixLen + 2 + (int)strlen(secondCommandList[0]));
                pLocal[0] = 0;
                //printf("plocal %s\n", pLocal);
                strncat(pLocal, &(arr[0]), prefixLen);
                strncat(pLocal, "//",1);
                strcat(pLocal, secondCommandList[0]);
                printf("Checking: %s\n", pLocal);
                //Checking to see if file exists
                if( access( pLocal, F_OK ) != -1 ) {
                    // file exists
                    printf("Found!\n");
                    printf("String = %s\n", secondCommandList[0]);
                    file2exists = true;
                    fname2 = malloc(sizeof(char)* strlen(pLocal));
                    fname2[0] = 0;
                    strcat(fname2, pLocal);
                    break;
                }
                else
                {
                    free(pLocal);
                }
            }
            if(file2exists == true && file1exists == true)
            {
                startProcess(fname, commandList, fname2, secondCommandList);
                free(fname);
                free(fname2);
            }
        }
    }
}


//Method for command Input from the user
int  commandInput ( char ** arrayOfPaths)
{
    //Getting user input
    //    command =  malloc(200 * sizeof(char *));
    command =  malloc(200);
    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    printf("Super Bash$ ");
    scanf(" %199[^\n]%*c",command);
    //Parsing the user command input
    if(strcmp(command, "exit\0") != 0)
    {
        char** pipe = parse (command, '|');
        char** commandList =  parse(pipe[0], ' ');
        char** pipeCommand = NULL;
        if(pipe[1] == NULL)
        {
            fileCheck(arrayOfPaths, commandList, NULL);
        }
        else if(pipe[1] != NULL)
        {
            pipeCommand =  parse(pipe[1],' ');
            fileCheck(arrayOfPaths, commandList, pipeCommand);
        }
        free(command);
        return 0;
    }
    return 1;
}

/*This is the main method*/
int main(int argc, char ** argv, char ** envp)
{
    char ** arrayOfPaths = NULL;
    char * pPath = NULL;
    
    //Get the path of environment pointer
    pPath = getPath(envp);
    
    //Parsing the path
    arrayOfPaths = parse(pPath, ':');
    do
    {
        runAgain  = commandInput(arrayOfPaths);
    }while(runAgain == 0);
    printf("\n!!!!!!!!!!!!!!!Good Bye!!!!!!!!!!!!!!!\n\n");
    return 0;
}

//Method for getting the Path

char * getPath(char ** envp)
{
    char * pPath = NULL;
    int found = 0;
    for (char ** env = envp; *env != 0 && found != 1; env++)
    {
        char* thisEnv = *env;
        char *current = thisEnv;
        int prefixLen = 0;
        int check = 0;
        char* path = NULL ;
        char delimeter = '=';
        while(check == 0)
        {
            if(*current == delimeter)
            {
                prefixLen =  (int)(current - *env);
                path =  malloc(prefixLen+1);
                strncpy(path, &thisEnv[0], prefixLen);
                check = 1;
            }
            current++;
        }
        //Comparing the strings
        if(strcmp( path,  "PATH") == 0)
        {
            printf ("%s\n",current);
            pPath = current;
            found = 1;
        }
    }
    //        char* path = getenv("PATH");
    //        printf ("%s\n",path);
    return pPath;
}

//Method for parsing the Array of Strings

char ** parse (char * string, char delimeter )
{
    int prefixLen = 0;
    int found = 0;
    char **listOfPaths = NULL;
    char *stringArray =  string;
    char *current =  stringArray;
    //Finding the total number of delimeters in the given string
    while(*current !=0)
    {
        if(*current == delimeter)
        {
            found++;
        }
        current++;
    }
    //printf("found = %d\n", found);
    if(found >0)
    {
        listOfPaths = malloc((found+2) *sizeof(char*));
        current = stringArray;
        int count = 0;
        //Looping through the string
        while(*current != 0 )
        {
            if(*current ==  delimeter)
            {
                prefixLen =  (int)(current - stringArray);
                listOfPaths[count] =  malloc(prefixLen+1);
                strncpy(listOfPaths[count], &stringArray[0], prefixLen);
                //                printf("%s\n",listOfPaths[count]);
                stringArray = ++current;
                count++;
                if(count == found)
                {
                    //printf("Count %d",count);
                    prefixLen =  (int)strlen(current) ;
                    if(prefixLen > 0)
                    {
                        listOfPaths[count] =  malloc(prefixLen+1);
                        strncpy(listOfPaths[count], &stringArray[0], prefixLen);
                    }
                    //                     printf("%s\n",listOfPaths[count]);
                }
            }
            current++;
        }
        //Setting the  last char pointer to null in the array of char pointers in the char  pointer pointer
        listOfPaths[++count] = NULL;
        
    }
    else if (found == 0)
    {
        listOfPaths = malloc( 2 * sizeof(char *));
        prefixLen = (int)(strlen(string)+1);
        listOfPaths[0] =  malloc(prefixLen);
        strncpy( listOfPaths[0], &string[0], prefixLen);
        listOfPaths[1] = NULL;
    }
    
    int newCount = 0;
    //Looping through the array of char pointers and printing the output
    for( newCount = 0 ; listOfPaths[newCount] != 0; newCount++)
    {
        printf(" %s   ", listOfPaths[newCount]);
        int strLength = 0;
        for(int i = 0; listOfPaths[newCount][i] != 0; i++)
        {
            strLength++;
        }
        printf(" Length  %d\n",strLength);
    }
    return listOfPaths;
}



