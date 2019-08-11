#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

char **tokenize(char *line)
{
    char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
    char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
    int i, tokenIndex = 0, tokenNo = 0;

    for(i =0; i < strlen(line); i++){

        char readChar = line[i];

        if (readChar == ' ' || readChar == '\n' || readChar == '\t') {
            token[tokenIndex] = '\0';
            if (tokenIndex != 0){
                tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
                strcpy(tokens[tokenNo++], token);
                tokenIndex = 0; 
            }
        } 
        else {
            token[tokenIndex++] = readChar;
        }
    }
 
    free(token);
    tokens[tokenNo] = NULL ;
    return tokens;
}

void sig_handler(int signum)
{
   printf("Interrupt SIGINT received.\n");
   return;
}


void main(void)
{
    char  line[MAX_INPUT_SIZE];            
    char  **tokens;              
    int i;
        
    while (1) 
    {           
    	signal(SIGINT,sig_handler);
      printf("Terminal$ ");     
      bzero(line, MAX_INPUT_SIZE);
      gets(line);
    	if(strcmp(line,"") == 0) continue;          
      printf("Got command %s\n", line);
      line[strlen(line)] = '\n'; //terminate with new line
      tokens = tokenize(line);
       
      //do whatever you want with the commands, here we just print them
    	
    	if(strcmp(tokens[0],"exit") == 0) 
      {
        exit(0);
      }
    	
    	else
    	{
      	int num_commands = 0;
        for(i=0;tokens[i]!=NULL;i++) num_commands++;
      	
      	int contains_semicolon = 0;
        int contains_redirection = 0;
      	int simple_command = 0;

      	for(int i=0;tokens[i]!=NULL;i++)
        {
      	 if(strcmp(tokens[i],";;") == 0)
          {
            contains_semicolon = 1;
            break;
          }
        }
              
        for(int i=0;tokens[i]!=NULL;i++)
        {
      	 if(strcmp(tokens[i],">") == 0)
          {
            contains_redirection = 1;
            break;
          }
        }
      	
      	if( (contains_semicolon == 0) && (contains_redirection == 0) ) simple_command = 1;
              
      	if(simple_command == 1 && strcmp(tokens[0],"cd") != 0)
        {
      	  int pid = fork();
      	  if(pid == 0)
          {
            int err = execvp(tokens[0],tokens);
          }
      	  else
          {
      	  int status;
          wait(&status);
          }
        }	

        if(simple_command == 1 && strcmp(tokens[0],"cd") == 0)
        {
          if(num_commands != 2) 
            {
              printf("Here1\n");
              printf("Incorrect number of arguements\n");
            }
          else
          {
          int cd = chdir(tokens[1]);
          if(cd == -1) printf("%s : No such file or directory.\n",tokens[1]);
          }
        }

        if(contains_semicolon == 1)
        {
          char **simple_command = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
          int j = 0;

          for(int i=0;i<num_commands;i++)
          {
            if(strcmp(tokens[i],";;") == 0)
            {
              if(strcmp(simple_command[0],"cd") == 0)
              {
                if(j != 2) 
                  {
                    printf("Incorrect number of arguements\n");
                  }
                else
                {
                int cd = chdir(simple_command[1]);
                if(cd == -1) printf("%s : No such file or directory.\n",simple_command[1]);
                }
                // for(int k=0;k<MAX_NUM_TOKENS;k++)
                // {
                //   simple_command[k] = NULL;
                // }
                j = 0;
              }
              else
              {
                int pid = fork();   
                if(pid == 0)
                {
                  // printf("%s %s\n",simple_command[0],simple_command[1]);
                  simple_command[j] = (char*) NULL;
                  int err = execvp(simple_command[0],simple_command);
                }
                else
                {
                  int status;
                  wait(&status);
                  for(int k=0;k<MAX_NUM_TOKENS;k++)
                  {
                    simple_command[k] = NULL;
                  }
                  j = 0;
                }
              }
            }

            else
            {
              simple_command[j] = tokens[i];
              j++;
            }
          }

          if(strcmp(simple_command[0],"cd") == 0)
          {
            if(j != 2) 
              {
                printf("Incorrect number of arguements\n");
              }
            else
            {
            int cd = chdir(simple_command[1]);
            if(cd == -1) printf("%s : No such file or directory.\n",simple_command[1]);
            }
            
            for(int k=0;k<MAX_NUM_TOKENS;k++)
            {
              simple_command[k] = NULL;
            }
            j = 0;
          }
          else
          {
            int pid = fork();   
            if(pid == 0)
            {
              // printf("%s %s\n",simple_command[0],simple_command[1]);
              simple_command[j] = (char*) NULL;
              int err = execvp(simple_command[0],simple_command);
            }
            else
            {
              int status;
              wait(&status);
              for(int k=0;k<MAX_NUM_TOKENS;k++)
              {
                simple_command[k] = NULL;
              }
              j = 0;
            }
          }

        }

        if(contains_redirection == 1)
        {
          char* filename = tokens[num_commands-1];
          char **simple_command = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
          int i = 0;
          while(strcmp(tokens[i],">") != 0)
          {
            simple_command[i] = tokens[i];
            i++;
          }
          simple_command[i] = NULL;

          int pid = fork();
          if(pid == 0)
          {
            int fid = open(filename,O_WRONLY|O_CREAT|O_TRUNC,S_IRWXO|S_IRWXU);
            dup2(fid,1);
            close(fid);
            int err = execvp(simple_command[0],simple_command);
          }
          else
          {
            int status;
            wait(&status);
          }
        }

      }

      // Freeing the allocated memory	
      for(i=0;tokens[i]!=NULL;i++)
      {
          free(tokens[i]);
      }
      free(tokens);
    }
}

                
