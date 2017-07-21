#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <process.h>
#include <unistd.h>

void arsh_loop();
char *arsh_readline(void);

char **arsh_split_line(char *line);

int arsh_cd(char **args);
int arsh_help(char **args);
int arsh_exit(char **args);

/*Names of bult in functions*/
char *builtin_str[] = {
	"cd",
	"help",
	"exit"
};

/*Built in functions*/
int (*arsh_builtin[]) (char **) = {
	arsh_cd,
	arsh_help,
	arsh_exit
};

/*Number of bult in functions*/
int arsh_num_builtin(void)
{
	return (sizeof(builtin_str)/sizeof(builtin_str[0]));
}

int arsh_execute(char **args);
	int arsh_launch(char **args);

int main(void)
{
	arsh_loop();

	return 0;
}

void arsh_loop()
{
	int status = 1;
	char *line;
	char **args;
	char **p;
	do
	{
		printf("> ");
		line = arsh_readline();
		args = arsh_split_line(line);
		p = args;
		while(*p++ != NULL)
		{
			printf("%s\n", *(p - 1));
		}
		status = arsh_execute(args);
	}while(status);
}

#define ARSH_RL_BUFFSIZE 1024
char *arsh_readline(void)
{
	int buffsize = ARSH_RL_BUFFSIZE;
	int i,c;
	char *buff = (char*)malloc(sizeof(char) * buffsize);
	if(!buff)
	{ 
		fprintf(stderr, "arsh_readline: allocation error\n"); 
		exit(EXIT_FAILURE); 
	}
	
	for(i = 0; (c = getchar()) != EOF && c != '\n'; i++)
	{
		if(i >= buffsize)
		{
			buffsize += ARSH_RL_BUFFSIZE;
			buff = (char*)realloc(buff, sizeof(char) * buffsize);
			if(!buff) 
			{ 
				fprintf(stderr ,"arsh_readline: allocation error\n"); 
				exit(EXIT_FAILURE); 
			}
		}
		buff[i] = c;
	}
	buff[i] = '\0';
	return buff;
}

#define ARSH_TOK_BUFSIZE 64
#define ARSH_TOK_DELIM " \t\r\n\a"
char **arsh_split_line(char *line)
{
	int bufsize = ARSH_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;

  	if (!tokens) 
	{
    	fprintf(stderr, "arsh: allocation error\n");
    	exit(EXIT_FAILURE);
  	}

  	token = strtok(line, ARSH_TOK_DELIM);
  	while(token != NULL)
	{
    	tokens[position++] = token;

	    if (position >= bufsize) 
		{
	    	bufsize += ARSH_TOK_BUFSIZE;
	    	tokens = realloc(tokens, bufsize * sizeof(char*));
	    	if (!tokens) 
			{
	        	fprintf(stderr, "arsh: allocation error\n");
	        	exit(EXIT_FAILURE);
	      	}
	    }
    	token = strtok(NULL, ARSH_TOK_DELIM);
  	}
  	tokens[position] = NULL;
  	return tokens;
}

int arsh_cd(char **args)
{
	if(args[1] == NULL)
	{
		fprintf(stderr, "arsh: expected argument to \"cd\"\n");
	}
	else if(chdir(args[1]) != 0)
	{
		perror("arsh");
	}
	return 1;
}

int arsh_help(char **args)
{
  	printf("ARSH\n");
  	printf("Type program names and arguments, and hit enter.\n");
  	printf("The following are built in:\n");

  	for (int i = 0; i < arsh_num_builtin(); i++) 
	{
    	printf("  %s\n", builtin_str[i]);
  	}

  	return 1;
}

int arsh_exit(char **args)
{
	return 0;
}

int arsh_execute(char **args)
{
	if(args[0] == NULL)
		return 1;
	
	for(int i = 0; i < arsh_num_builtin(); i++)
	{
		if(strcmp(args[0], builtin_str[i]) == 0)
		{
			return (*arsh_builtin[i])(args);
		}
	}
	
	arsh_launch(args);
}

int arsh_launch(char **args)
{
	if(spawnv( P_WAIT, args[0], args) < 0)
	{
		perror("arsh");
	}
	return 1;
}
