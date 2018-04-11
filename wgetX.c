/*Frabec*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "url.h"
#include "wgetX.h"

int main(int argc, char* argv[])
{

	url_info info;


	if (argc != 2) {
		exit_with_error("The wgetX must have exactly 1 parameter as input. \n");
	}
	char *url = argv[1];

	

	//get the url
	parse_url(url, &info);
	if (info.validity==1){
		exit_with_error("Unvalid URL");
	}

	printf("Downloading %s \n", url);

	//	print_url_info(info);

	//download page
	char *recv_buf_t;
	recv_buf_t = malloc(sizeof(char)*B_SIZE);
	bzero(recv_buf_t, sizeof(recv_buf_t));
	char *buff = download_page(info, recv_buf_t);
	char *page= read_http_reply(buff);
	//write to the file
	write_data("received_page", page);

	free(recv_buf_t);

	puts("the file is saved in received_page.");
	return (EXIT_SUCCESS);
}

char* download_page(url_info info, char *recv_buf_t)
{
	print_url_info(info);
	int mysocket=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in addrport;
	addrport.sin_family = AF_INET;
	addrport.sin_port = htons(info.port);
	struct hostent *infos_serveur=gethostbyname(info.host);
	memcpy(&addrport.sin_addr.s_addr, infos_serveur->h_addr, infos_serveur->h_length);
	if (connect(mysocket, (struct sockaddr *)&addrport, sizeof(addrport) )<0){
		exit_with_error("Unable to connect");
	}
	puts("Connected !");
	char *toSend= http_get_request(info.path,info.host);
	if(send(mysocket,toSend, strlen(toSend),0)<0){
		exit_with_error("Unable to send request");
	}
	if (recv(mysocket, recv_buf_t, sizeof(char)*B_SIZE,0)<0){
		exit_with_error("The server doesn't reply");
	}
	close(mysocket);
	return recv_buf_t;
}

void write_data(const char *path, const char * data)
{
	FILE* fichier= NULL;
	fichier=fopen(path, "w");
	if (fichier!=NULL){
		fputs(data,fichier);
	}
	else {
		exit_with_error("Impossible d'ouvrir le fichier");
	}
	fclose(fichier);
}

char* http_get_request(char* path, char* host) {
	//char request_buffer[1024];
	char * request_buffer = (char *) malloc(1024);
	memset(request_buffer, 0, sizeof(*request_buffer));
	snprintf(request_buffer, 1024, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
			path, host);
	return request_buffer;
}


char* read_http_reply(char* recv_buf_t) {
	//first line, get the status code
	char *status_line = strstr(recv_buf_t, "\r\n");
	*status_line = '\0';
	//		puts(recv_buf_t);
	char status[4];
	memcpy(status, recv_buf_t + 9, 3); //get the status string
	status[3] = '\0';
	int status_code = atoi(status);

	recv_buf_t = status_line + 2; //now move to the next line.

	char* page;
	switch(status_code){
	case 200: //all ok

		page = strstr(recv_buf_t, "\r\n\r\n");
		*page = '\0';
		recv_buf_t = page + 4; //now the recv_buf pointer is pointing to the begin of the document

		break;
	case 302: //redirect
		//do the redirect here
		break;

	}
	return recv_buf_t;
}