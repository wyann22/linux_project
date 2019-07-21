#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
char* my_getline(int* max_size){   
      char* line = (char*)malloc( sizeof(char) * (*max_size));
      if(line==NULL){
		printf("error\n" );
		exit(1);
	}
	int c;
	int len = 0;
	while( (c = getchar()) != EOF ){
		if(len>=*max_size){
			*max_size =2*(*max_size);
			line = realloc(line, sizeof(char)*(*max_size));
			if(line==NULL){
			printf("error\n" );
			free(line);
			exit(1);
			}
		}
		line[len++] = c;
		if('\n' == c){
			break;
		}
	}
	if(len>0&&line[len-1]!='\n'){
		if(len==*max_size){
			*max_size = *max_size+1;
			line = realloc(line, sizeof(char)*(*max_size));
			if(line==NULL){
				printf("error\n" );
				free(line);
				exit(1);
			}
		}
		line[len]='\n'; 
		len = len+1 ; 	
	}
	if(len==*max_size){
		*max_size = *max_size+1;
		line = realloc(line, sizeof(char)*(*max_size));
		if(line==NULL){
			printf("error\n" );
			free(line);
			exit(1);
		}
	}
	line[len] = '\0';
	//free(line);
	if(len==0){
		return NULL;
	}
	return line ; 

}

void sort(char** arr,int length){
	char *temp=NULL;
	  for(int i = 0 ; i <length ; i++){
	  	for(int j = i ; j >0;j--){
	  		if(strcmp(arr[j],arr[j-1])<=0){
	  			
	  			temp=arr[j] ;
	  			arr[j] =arr[j-1];
	  			arr[j-1] =temp; 
	  		}else{
	  			break ; 
	  		}
	  	}
	  }
}
void swap(char ** arr , int i , int j){
	    char* temp =NULL;
	     temp=arr[j] ;
	     arr[j] =arr[i];
	     arr[i] =temp; 
}

int partition (char **arr, int low, int high){
    // pivot (Element to be placed at right position)
    char * pivot = arr[high];  
    int i = (low - 1);  
 // Index of smaller element
    for (int j = low; j <= high- 1; j++){
        // If current element is smaller than or
        // equal to pivot
        if (strcmp(arr[j],pivot)<=0){
            i++;    // increment index of smaller element
            swap(arr,i,j);
        }
    }
    swap(arr,i + 1,high);
    return i + 1;
}
void quickSort(char ** arr, int low, int high){
    if (low < high){
        /* pi is partitioning index, arr[p] is now
           at right place */
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);  // Before pi
        quickSort(arr, pi + 1, high); // After pi
    }
}
int main(int argc, char ** argv) {

	int line_max_size = 2;
	int lines_max_size = 2 ; 
	char** lines = (char **) malloc(sizeof(char*)*lines_max_size);
	if(lines==NULL){
		printf("error\n" );
		free(lines);
		exit(1);
	}
	char * line = NULL ;
	int len =0;  
	while( (line  = my_getline(&line_max_size))!=NULL){
		if(len>=lines_max_size){
			lines_max_size = lines_max_size*2 ; 
			lines = realloc(lines, sizeof(char*)*lines_max_size);	
			if(lines==NULL){
				printf("error\n" );
				free(lines);
				exit(1);
			}	
		}
		lines[len++]=line; 
		//printf("%s", line);	
	}
	quickSort(lines,0,len-1);
	//sort(lines,len);
     for (int i = 0; i < len; ++i)
     {
     	 printf("%s",lines[i] );
     	 free(lines[i]);
     }
	free(line);
	free(lines);
}

	
	
