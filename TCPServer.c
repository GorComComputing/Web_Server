// WebServer (TCP HTTP)
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080

int main(int argc, char const *argv[]){
	int server_fd;
	int new_socket;
	long valread;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	
	char *hello = "HTTP/1.1 200 OK\n" 
	"Content-Type: text/html\n" 
	"Content-Length: 120\n" 
	"\n"; 
	//"Hello from server!";
	
	

	FILE *fp;	// указатель на файл
	char name[] = "index.html";	// имя файла по умолчанию
		
	// Открываем файл
	if((fp = fopen(name, "r")) == NULL){
		printf("Error open file\n");
		return -1;
	}
	
	// узнаем размер файла для создания буфера нужного размера
	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	
	// выделяем память под буфер
	char* buf = (char*)malloc(sizeof(char)*size);
	//читаем полностью весь файл в буфер
	fread(buf, 1, size, fp);
	printf("%s\n", buf);
	
	
	
	// закрываем файл после прочтения
	fclose(fp);
	
	
	// склеиваем строки
	size_t len1 = strlen(hello);
	char *result = malloc(len1 + size);
	if(!result){
		printf("malloc() failed");
		return -1;
	}
	
	memcpy(result, hello, len1);
	memcpy(result + len1, buf, size + 1);
	
	printf("%s\n", result);
	
	printf("%ld\n", size);
	
	// Создаем соккет
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
		perror("In socket");
		exit(EXIT_FAILURE);
	}
	
	
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	
	memset(address.sin_zero, '\0', sizeof address.sin_zero);
	
	
	if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
		perror("In bind");
		exit(EXIT_FAILURE);	
	}
	
	if(listen(server_fd, 10) < 0){
		perror("In listen");
		exit(EXIT_FAILURE);
	}
	
	while(1){
		printf("\n++++++ Waiting for new connection ++++++\n\n");
		if((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0){
			perror("In accept");
			exit(EXIT_FAILURE);
		}
		
		char buffer[30000] = {0};
		valread = read(new_socket, buffer, 30000);
		printf("%s\n",buffer);
		write(new_socket, result, strlen(result));
		printf("-----Hello message sent-----\n");
		close(new_socket);
	}
	
	
	// освобождаем память от буфера с файлом
	free(buf);
	return 0;
}
