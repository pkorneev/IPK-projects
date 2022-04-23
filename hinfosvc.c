#include<stdio.h>
#include<string.h>	
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>	
#include<unistd.h>

#define HTTPCONNECT "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n"
#define BAD_REQ "400 Bad request\n"

//Print cpu name
void print_cpu_info (int new_socket) 
{
	char cpu_info[100];
	FILE *cpu = popen("cat /proc/cpuinfo | grep 'model name' | head -n 1 | awk -F ': ' '{print $2}'", "r");
	fgets(cpu_info, 100, cpu);
	write(new_socket, cpu_info, strlen(cpu_info));
	return;
}
//Print hostname 
void print_hostname(int new_socket)
{
	int hostname;
	char hostbuffer[64];
	hostname = gethostname(hostbuffer, sizeof(hostbuffer));
	if(!hostname)strcat(hostbuffer,"\n");
	write(new_socket , hostbuffer , strlen(hostbuffer));
	return;
}
//Print cpu usage in % 
void print_cpu_load(int new_socket){
    
	unsigned long long int PrevIdle, Idle, PrevNonIdle, NonIdle, PrevTotal, Total, idled, totald;
	unsigned long long int previdle, previowait, idle, iowait;
	unsigned long long int prevuser, prevnice, prevsystem, previrq, prevsoftirq, prevsteal;
	unsigned long long int user, nice, system, irq, softirq, steal; 
	double cpu_percentage;
	char load_percentage[10];
	char name[5];
	char name2[5];
    FILE* fp = popen("cat /proc/stat | grep 'cpu' ","r");
    if(fp != NULL){
		fscanf(fp, "%s %llu %llu %llu %llu %llu %llu %llu %llu", name, &prevuser, &prevnice, &prevsystem, &previdle, &previowait, &previrq, &prevsoftirq, &prevsteal);
	}
	else{
		exit(EXIT_FAILURE);
	}
	PrevIdle = previdle + previowait;
	PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal;
	PrevTotal = PrevIdle + PrevNonIdle;
	fclose(fp);
	sleep(1);
	 FILE* fp2 = popen("cat /proc/stat | grep 'cpu' ","r");
    if(fp != NULL){
		fscanf(fp, "%s %llu %llu %llu %llu %llu %llu %llu %llu", name2, &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
	}
	else{
		exit(EXIT_FAILURE);
	}
	Idle = idle + iowait;
	NonIdle = user + nice + system + irq + softirq + steal;
	Total = Idle + NonIdle;
	totald = Total - PrevTotal;
	idled = Idle - PrevIdle;
	cpu_percentage = (double)(totald - idled)/totald *100;
	snprintf(load_percentage, 10, "%.2f", cpu_percentage); //Convert double to string
	strcat(load_percentage, "%\n");//Add % and \n to our output
    fclose(fp2);
	write(new_socket, load_percentage, strlen(load_percentage));	
	return;
}

int main(int argc , char *argv[])
{
	int socket_desc , new_socket , c, accepted, port_number;
	struct sockaddr_in server , client;
	char req[1500];
    

    //Check the cl argumets 
    if (argc != 2) {
       fprintf(stderr,"usage: %s <port>\n", argv[0]);
       exit(EXIT_FAILURE);
    }

    port_number = atoi(argv[1]);
    
	
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc < 0)
	{
		perror("ERROR: could not create socket");
        exit(EXIT_FAILURE);
	}
    int optval = 1;
    setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port_number);
	
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("ERROR: binding");
        exit(EXIT_FAILURE);
	}
	
	//Listen
	listen(socket_desc , 3);
	
	c = sizeof(struct sockaddr_in);
	while(1)
	{
        new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
		if (new_socket<0)
	    {
		    perror("accept failed");
		    return 1;
	    }

		accepted = recv(new_socket, req, 1500, 0);	
		write(new_socket, HTTPCONNECT,strlen(HTTPCONNECT));		

    
		 if (strncmp (req, "GET /hostname ",strlen("GET /hostname") + 1) == 0) // comapre 2 strings to call the func
    	{
		   print_hostname(new_socket);
	    }
		else if (strncmp(req, "GET /cpu-name ", strlen("GET /cpu-name") + 1) == 0)
		{
			print_cpu_info(new_socket);
		}
		else if (strncmp(req, "GET /load ", strlen("GET /load") + 1) == 0)
		{
			print_cpu_load(new_socket);
		}
		else{
			write(new_socket, BAD_REQ, strlen(BAD_REQ));
		}

		close(new_socket);
	}
	
	return 0;
}  