/*
CPU INFO: /proc/cpuinfo  command:lscpu
KERNEL INFO: /proc/kmsg  syscall:syslog
load averages: /proc/loadavg command: uptime 
memory info: /proc/meminfo 
system statistics: /proc/stat (cpu time, number of context switch, process ...)

*/

#include <stdio.h>
#include <sys/time.h>

#define STANDRAD ''
#define SHORTE 's'
#define LONGE 'l'
//enum r_type{Standrad,Short,Long}

void error_usage(){
	fprintf(stderr, "usage: observer [-s][-l int dur]\n");
	exit(1);
}

int get_time(){
	timeval now; 
	gettimeofday(&now,NULL);
	printf("Linux observer at %s\n",ctime(&(now.tv_sec)));
	return 0;
}

int short_report(){
	r_cpu();
	r_kernel();
	r_time();
	return 0;
}
int main(int argc, char const *argv[])
{
	char c1,c2;
	if(argc>1){
		sscanf(argv[1],"%c%c",&c1,&c2);
		get_time();
		if(c1!='-'){
			error_usage();
		}else if(c2==SHORTE){
			short_report()
		}else if(c2==LONGE){
			int interval=atoi(argv[2]);
			int duration = atoi(argv[3]);
			long_report(interval,duration);
		}else{
			error_usage();
		}
	}else{
		standrad_report();
	}


	return 0;
}

int r_cpu(){
	FILE* proc_file = fopen("/proc/cpuinfo","r");
	
	fgets()
	printf("%s\n", );
}



