/**
 *  Jiazi Yi
 *
 * LIX, Ecole Polytechnique
 * jiazi.yi@polytechnique.edu
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"url.h"



/**
 * parse a URL and store the information in info.
 */
int Valid=0;

void parse_url(char* url, url_info *info)
{
	// url format: [http://]<hostname>[:<port>]/<path>
	// e.g. https://www.polytechnique.edu:80/index.php

	char *protocol, *host_name_path;
	info->url=url;
	protocol = strstr(url, "://");
	// it searches :// in the string. if exists,
	//return the pointer to the first char of the match string

	if(protocol){ //protocol type exists
		*protocol = '\0'; //end of the protocol type
		info->protocol=strtok(url, "\0");
		host_name_path = protocol + 3; //jump to the host name
		
	} else {	//no protocol type: using http by default.
		host_name_path = url;
		url = "http";
		info->protocol=url;
	}


	
	
	printf("host_name_path: %s\n", host_name_path);
	
	if (!strstr(host_name_path,"/")){
		Valid=1;
	}
	else{
		
		if (strstr(host_name_path,":"))
		{
			char *token1 = strtok(host_name_path, ":"); //search the ":" in the host_name_path

			char *token2 = strtok(NULL, "/"); //putting the first argument as NULL means continuing the same search
			char *path_token = strtok(NULL, ""); //all the rest; get the path
			char *token2bis=token2;
			//We check if the port contains only integers.
			while(*token2bis!='\0'){
				printf("%d\n",*token2bis);
				if (*token2bis<48 || *token2bis>57){
					Valid=1;
				}
				else{
				}
				token2bis++;
			}
			info->port=atoi(token2);
			info->host=token1;
			info->path=path_token;

		}

		else {
			char *token3 = strtok(host_name_path, "/");
			char *token4= strtok (NULL, "");
			info->port=80;
			info->host=token3;
			info->path=token4;
		}
	}	

}

/**
 * print the url info to std output
 */
void print_url_info(url_info info){
	printf("The URL contains following information: \n");
	printf("Protocol type:\t%s\n", info.protocol);
	printf("Host name:\t%s\n", info.host);
	printf("Port No.:\t%d\n", info.port);
	printf("Path:\t\t%s\n", info.path);
}

/**
 * exit with an error message
 */

void exit_with_error(char *message)
{
	fprintf(stderr, "%s\n", message);
	exit(EXIT_FAILURE);
}

int validate_url(url_info *info){
	if (!info->protocol || !info->host){
		return 1;
	}

	if (Valid==1){
		return 1;
	}
	if (strstr(info->protocol,"/")||strstr(info->host,"/")||strstr(info->protocol,":")||strstr(info->host,":")){
		return 1;
	}

	return 0;
}