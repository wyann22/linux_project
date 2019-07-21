#include <stdio.h>
#include <stdlib.h>
#include <string.h>	
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>	
/*
	int file = open("empty",O_RDONLY);
	char buffer[11];
	int count = 10 ; 
	read(file,buffer,count);
	printf("%s\n",buffer );
	*/
typedef struct command_struct{
	char name[30]; 
	char **argv;
	char path[20][100]; 
}Command;
int  read_from_stdin(Command*);
void print_to_stdout(char*);
void execute_command(Command*);
void free_all(Command*);
//void initialize(Command*)
void find_file(Command*,char* file_path);
void get_path(Command*);
int file_exist(char*);


int main(){
	Command command;
	//initialize(&command);
	char * prompt_string = "my-shell>";
	//Main loop
	get_path(&command);
	while(!feof(stdin)){
		print_to_stdout(prompt_string);
		if(read_from_stdin(&command)==1){
			execute_command(&command);
		}
		
	}
	//free_all(&command);
	return 0 ;	
	//Terminate my shell. 
}
int file_exist( char* filename){
    struct stat buffer;
    int exist = stat(filename,&buffer);
    if(exist == 0)
        return 1;
    else // -1
        return 0;
}

/*void initialize(Command *command){
	//command->path=(char**)malloc(sizeof(char**));
	*(command->path)=(char*)malloc(sizeof(char*));	
}*/	
//Determin the command name and params
int read_from_stdin(Command* command){
	char  *str=NULL;
	size_t len = 200;
	char delim[] = " ";
	char * token; 
	size_t char_num = getline(&str,&len,stdin);
	//char ** command->argv = command->argv;
	if(char_num<=0){
		fprintf(stderr, "Error reading from stdin!\n");
		exit(1);
	}else{
		str[strlen(str)-1]=0;
	}
	token = strtok(str,delim);
	if(token==NULL){
		return 0 ; 
	}
	strcpy(command->name,token);
	command->argv =(char **)malloc(sizeof(char*));
	*(command->argv)=(char*)malloc(sizeof(char)*strlen(token));
	strcpy(*(command->argv),token);
	int i; 
	for(i=1;;i++){	
		token = strtok(NULL,delim);
		if(token==NULL){
			break;
		}
		command->argv=(char**)realloc(command->argv,sizeof(char*)*(i+1));
		*(command->argv+i)=(char*)malloc(sizeof(char)*strlen(token));
		strcpy(*(command->argv+i),token);
		printf("%s\n",*((command->argv)+i));
	
			
	}
	//(char*)*(command->argv+i)=NULL;
	return 1 ; 
		 
	//scanf("%c%c%c",command->name,command->argv[0],command->argv[1]);
}
//Find the full file path based on the name.
void find_file(Command *command,char * file_path){
	char * file_name = command->name;
	if(file_exist(file_name)==1){
		return ; 
	}
	int i =0 ; 
	char temp_path[150];
	while(*(command->path+i)!=NULL){
		strcpy(temp_path,"");
		strcat(temp_path,*(command->path+i));
		strcat(temp_path,"/");
		strcat(temp_path,file_name);
		if(file_exist(temp_path)==1){
			strcpy(file_path,temp_path)	;
			printf("%s\n",temp_path);

			return ;	
		}
		i++;
	}
}

// Launch the executable file with specified parameters. 

void execute_command(Command *command){
	int status;
	char file_name[150] ;
	find_file(command,file_name);
	int i =0; 
    /*while(*((command->argv)+i)!=NULL){
		printf("mmm%s\n",*((command->argv)+i));
		i++;
	}
	i=0;*/

			printf("before\n");

	if(fork()==0){
		printf("brefore\n");
		execve(file_name,command->argv,0);
		printf("after\n");
	}else{
		wait(&status);
		while(*((command->argv)+i)!=NULL){
			printf("mmmd%s\n",*((command->argv)+i));
			free(*((command->argv)+i));
			i++;
			//command->argv[i]="";
		}
		free(command->argv);
		//command->argv=NULL;
	}
}
void get_path(Command *command){
	char * env_str = getenv("PATH");
	char delim[]= ":";
	char * token = strtok(env_str,delim);
	int i = 0 ;
	while(token!=NULL){
		 strcpy(command->path[i],token);
		 printf("%s\n",*(command->path+i) );
		 i++;
		token = strtok(NULL,delim);
	}
}
void print_to_stdout(char * string){
	printf("%s",string);
}