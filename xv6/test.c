#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char **argv){
	int pid=10;
	
	printf(2,"init's nice vlaue %d\n", getnice(1));//1
	printf(2,"non-existing's nice vlaue %d\n", getnice(-1));//2
	printf(2,"setnice value of non-existing %d\n", setnice(-1,10));//4
	printf(2,"setnice's wrong nice vlaue return %d\n", setnice(getpid(), 70));//5
	printf(2,"setnice's wrong nice vlaue %d\n",getnice(getpid()));
	printf(2,"setnice current's nice vlaue %d\n", setnice(getpid(), 35));
	printf(2,"setnice current's nice vlaue %d\n",getnice(getpid()));
	
	if((pid=fork()) == 0){
		printf(2,"child's nice value %d\n", getnice(getpid()));
	}else{
		printf(2,"parent's nice value %d\n", getnice(getpid()));
	}

	exit();	
}
