#include <stdio.h>
#include <sys/syscall.h>

int main(){
int x=9;
syscall(401,10);
int i=0;
while(1){
	printf("2\n");
}
return 0;
}

