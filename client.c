
#include "serverClient.h"

void		sendFileName(int clientSocket, char *argv)
{
	char		*name;

	// remove path to file from file name
	name = strrchr(argv, '/') + 1;
	printf("File '%s' ", name);
	if (send(clientSocket, name, strlen(name), 0) <= 0)
		errorMsg("can't send file");
}

static void		setServerInfo(struct sockaddr_in *socketAddr)
{
	socketAddr->sin_family = AF_INET;
	socketAddr->sin_port = htons(SERVER_PORT);
	//	socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_pton(AF_INET, SERVER_IP, &(socketAddr->sin_addr));
}

int main(int argc, char *argv[])
{
	char		buf[READ_SIZE] = {0};
	int		fd;
	int		clientSocket;
	struct	sockaddr_in socketAddr;
	size_t	readed;
	size_t	sended;

	if (argc != 2)
		errorMsg("Wrong number of parameters!");
	
	fd = open(argv[1], O_RDONLY);
	if ((read(fd, buf, 0) == -1) || (fd < 0))
		errorMsg("Can't open file!");

	// 1. Create socket for incoming connections
	if ((clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		errorMsg("socket() failed");
	
	// 2. set IP/PORT of Server. CONNECT
	setServerInfo(&socketAddr);
	
	if (connect(clientSocket, (struct sockaddr *) &socketAddr, sizeof(socketAddr)) < 0)
		errorMsg("connect() failed");
	printf(COLOR_GREEN"Connected to server [%s : %d]\n" COLOR_RESET, SERVER_IP, SERVER_PORT);
	
	sendFileName(clientSocket, argv[1]);

	while ((readed = read(fd, buf, READ_SIZE - 1)) > 0)
	{
		sended = send(clientSocket, buf, readed, 0);
		if (sended <= 0)
			errorMsg("can't send");
	}
	close(fd);
	printf("successfully send!\n");
	printf(COLOR_RED"Disconnected from server [%s : %d] \n"COLOR_RESET, SERVER_IP, SERVER_PORT);
	shutdown(clientSocket, SHUT_RDWR);
	return 0;
}
