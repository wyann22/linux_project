#include <stdio.h>
#include <stdlib.h>
#include <string.h>	
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>	
#define MAX_COMMAND_SIZE 100
#define MAX_ARGC 10
typedef struct command_struct{
	char * line ;
	char name[MAX_COMMAND_SIZE]; 
	char **argv;
	int argc;  
}Command;

int get_line(char * dest, int max_size ){
	char *ch = dest ;
	int count=0; 
	while((*ch=getchar())!=EOF&&count<=max_size){
		if(*ch=='\n')
			break;
		ch++;
		count++;
	}
	if(*ch==EOF||count==0){
		//printf("asd\n");
		return 0;
	}
	*ch=0;
	return 1; 
}
Command* read_command(){
	Command* command ;
	char line[MAX_COMMAND_SIZE];
	char* ch;
	int i=1; 
	if(get_line(line,MAX_COMMAND_SIZE)==1){
		command= (Command*)malloc(sizeof(Command));
		command->line = (char*)malloc(sizeof(char)*MAX_COMMAND_SIZE);
		strcpy(command->line,line); 
		for(ch = command->line;*ch!=0;ch++){
			if(*ch!=' '&&*ch!='\t'){
				command->argv=(char**)malloc(sizeof(char*)*MAX_ARGC);
				command->argv[0] = ch;
				break; 
			}
		}
		for(ch=ch+1;*ch!=0;ch++){
			if(*ch==' '||*ch=='\t'){
				*ch=0;
				ch++;
				break;
			}
		}
		while(*ch!=0){
			for(;*ch!=0;ch++){
				if(*ch!=' '&&*ch!='\t'){
					command->argv[i]=ch;
					i++;
					break;
				}
			}
			for(ch=ch+1;*ch!=0;ch++){
				if(*ch==' '||*ch=='\t'){
					*ch=0;
					ch++;
					break;
				}
			}
		
		}
		return command;
	}else{
		return NULL ; 
	}
}
void free_command(Command * command){
	free(command->line);
	free(command->argv);
	free(command);
}
int file_exist( char* filename){
    struct stat buffer;
    int exist = stat(filename,&buffer);
    if(exist == 0)
        return 1;
    else // -1
        return 0;
}
int find_path(Command* command){
	char * file_name =command->argv[0];
	char temp_path[MAX_COMMAND_SIZE];
	char *ch;
	char *start;
	char* envr= getenv("PATH");
	char  env[strlen(envr)+1];
	strcpy(env,envr);
	strcat(env,":");
	printf("%s\n",env );
	if(file_exist(file_name)){
		strcpy(command->name,file_name);
		return 1;
	}
	if(strlen(envr)==0){
		return 0;
	}
	ch = env;start=env;
	while(*ch!=0){
		if(*ch==':'){
			*ch=0;
			sprintf(temp_path,"%s/%s",start,file_name);
			printf("%s\n",temp_path);
			if(file_exist(temp_path)){
				strcpy(command->name,temp_path);
				return 1;
			}
			memset(temp_path,0,MAX_COMMAND_SIZE);
			start=++ch;
		}else{
			ch++;
		}
		
	}
	return 0;
			
}
int execute_command(Command *command){
	int status;
	//char file_name[150] ;
	//find_file(command,file_name);
	char *file_name=command->name;
	if(fork()==0){
		execve(file_name,command->argv,0);
	}else{
		wait(&status);
		if(status<0){
			return 0;
		}else{
			return 1;
		}
	}
}
int main(){
	Command *command;
	//initialize(&command);
	char * prompt_string = "my-shell>";
	char **ch;
	//Main loop
	//get_path(&command);
	while(!feof(stdin)){
		printf("%s",prompt_string);
		if((command=read_command())!=NULL){
			/*printf("%s\n",command->name);
			for(ch=command->argv;*ch!=NULL;ch++){
				printf("%s\n",*ch);
			}*/
			if(find_path(command)==1){
				if(execute_command(command)==0){
					printf("Invalid command!\n");
				}
			}else{
				printf("Command not exist!\n");
			}
			free_command(command);	
		}
		
	}
	return 0 ;	
	//Terminate my shell. 
}