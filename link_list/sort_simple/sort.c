#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
void sort(char arr[20][22],int length){
	  for(int i = 0 ; i <length ; i++){
	  	for(int j = i ; j >0;j--){
	  		if(strcmp(arr[j],arr[j-1])<=0){
	  			char temp[22];
	  			strcpy(temp,arr[j]) ;
	  			strcpy(arr[j] ,arr[j-1]);
	  			strcpy(arr[j-1] ,temp); 
	  		}else{
	  			break ; 
	  		}
	  	}
	  }
}
int main(int argc, char ** argv) {
	// TODO: Implement your code here!
	//char words[20][22];
	//int i,j =0;
	char word[22];
	char words[20][22];
	int i = 0 ;
	while(fgets(word,22,stdin)){
		int str_len = strlen(word) ; 
		//printf("%d\n",str_len );
		if(word[str_len-1]!='\n'){
			word[str_len]='\n';
			word[str_len+1]='\0';
		}
		strcpy(words[i],word);
		i++;
	}
	/*while(1){
		char letter = getchar() ; 
		if(isprint(letter)&&letter){
			if(letter=='\n'){
				word[j]=letter;

				strcpy(words[i] , word);
				printf("%s\n",words[i] );
				i++ ;
				//word = "";
				j=0;
			}else{
				word[j]=letter;
				j++;  
			}
		}else{
			break ; 
		}
	}*/
/*      while(j<20){
		char word[20];
		char letter = getchar();
		if(!isprint(letter)){
			break ;
		}
	       int i =0;
	       while(letter!='\n'){
	     		word[i]=letter;
	     		letter = getchar();
	     		if(!isprint(letter)){
	     			break ;
	     		}
	     		i++;
	       } 
	       word[i] = '\0';
	       strcpy(words[j],word);
	       j++;
      }*/
      int length = i ; 
      sort(words,length);
     for (int i = 0; i < length; ++i)
     {
     	printf("%s",words[i] );
     }
      return 0;
}


	
	
