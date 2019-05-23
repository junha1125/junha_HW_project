#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>

#define MAX_LEN_LINE    30
#define MultiLimit	10
#define LEN_HOSTNAME	30


void DivideString(char *a,int *c,int index[MultiLimit]){
	for(int i = 0;i < MAX_LEN_LINE; i++){ 
		if(*(a+i)==';'){
			index[0] = i;
			break;
		}
	}
			
	for(int j =1;j < MultiLimit;j++){
		for(int i =index[j-1]+1; i < MAX_LEN_LINE; i++){
			if(*(a+i)==';'){
				index[j] = i;
				break;
			}
			if(*(a+i)=='\0'){
				index[j] = i;
				*c = j;
				break;
			}
		}
		// printf("%d %d %d %d %d %d\n",index[0],index[1],index[2],index[3],index[4],*c);
		if(a[index[j]]=='\0') break;
	}
}







void strTostr(char *command,char *args[MultiLimit],int *c,int index[MultiLimit]){
	for(int i = 0 ; i < *c; i++){
		command[index[i]] = '\0';	
	}
	printf("process[0] : [%s]\n",args[0]);
	for(int i = 1 ; i < *c+1; i++){
		args[i] = &command[index[i-1]+2];
		printf("process[%d] : [%s]\n",i,args[i]);
	}

}






int Is_semicolone(char *a){
	for(int i = 0 ; i < MAX_LEN_LINE; i++){
		if((int)a[i] == 59){  //semicolon ASCII == 59
		return 1;
		}
	}
	return 0;

}



void ForkCopy(char *b[MultiLimit],const int n){
   	int ret, status;
    	pid_t pid, cpid;

        pid = fork(); // pid_parent, pid_child 따로 저장됨
	if (pid != 0){
	printf("▶- Runing PID[%d] Program --\n",pid);
	}

        if (pid < 0) {
            fprintf(stderr, "fork failed\n");
            exit(1);
        } // 어차피 실행 안되는 if문
 

        if (pid != 0) {  /* parent */
            cpid = waitpid(pid, &status, 0); // pid_parent == child process PID 
				             //이므로 parent Process인 myshell은 계속 기다린다.
					     // 즉 아래의 else문이 끝나때까지 아래코드 실행 안함
            if (cpid != pid) {
                fprintf(stderr, "waitpid failed\n");        
            }
            printf("Child process terminated\n");
            if (WIFEXITED(status)) {
                printf("-- Exit status is %d --\n", WEXITSTATUS(status)); 
            }
         }


        else {  /* child */
           ret = execve(b[n], &b[n], NULL);
           if (ret < 0) {
               fprintf(stderr, "--   execve failed   --\n"); 
           }
        } 
}




void printfHostname(void){
	char hostname[LEN_HOSTNAME + 1];
	memset(hostname, 0x00, sizeof(hostname));
	gethostname(hostname, LEN_HOSTNAME);
	printf("%s@%s$ ", getpwuid(getuid())->pw_name,hostname);
}







int main(void){
    while (true) {
	char command[MAX_LEN_LINE] = "00000000000000000000000000000";
	char *args[MultiLimit] = {command, NULL,NULL,NULL,NULL};
	printf("-------------------------NEW SHELL------------------------\n");
	printf("|	    We can MultiProcess sequentially.		 |\n");
	printf("| Use [;+space] to divide processes. ex)[hello; aaa; cc] |\n");
	rewind(stdin);
	// 2차원포인터가 아니고 1차원 다중 포인터 이다.
 	int n = 0;
	int multiN = 0;
	int index[MultiLimit] = {0,0,0,0,0};

        char *s;
        int len;
        
        printfHostname();
        s = fgets(command, MAX_LEN_LINE, stdin);
	// 여기서 멈추는 중
	
	// 아래의 if문은 실행될 가능성 없음 enter자체가 '\n' 이기 때문에 
        if (s == NULL) {
            fprintf(stderr, "fgets failed\n");
            exit(1);
        }
        

        len = strlen(command);
        // printf("%d\n", len); // $뒤에 친 문자열의 문자수 출력 ('\n'까지 포함한 숫자)
        if (command[len - 1] == '\n') {
            command[len - 1] = '\0'; 
        }
        
        printf("Your Input : [%s]\n", command); // '\0'을 찾고, 바로 이전 문자까지 문자열 출력

	// *****  Test 1. exit   *****	
	if(strcmp(command,"exit") == 0){
	break;	
	}

	// *****  Test 2. a; b; c
	if(Is_semicolone(command) == 1){
		printf("We are going to MultiProcess...\n");
		DivideString(command,&multiN,index);
		strTostr(command,args,&multiN,index);
		for(int i = 0;i < multiN+1;i++){
		ForkCopy(args,i);
		}
	}
	else{
	ForkCopy(args,0);
	}


    } // end infinit Loof
    return 0;
}




/*
질문 1 : [/bin/ls; /bin/ls]  [/bin/ls; /bin/ls; /bin/ls]에서 왜 마지막 것만 실행되는가? [aa; b; c]는 되는데
질문 2 : strcmp는 원래 오류가 많은 함수 인가요?
질문 3 : [aa; /bin/ls]는 잘되지만, [/bin/ls; aa]는 왜안될까..






*/
