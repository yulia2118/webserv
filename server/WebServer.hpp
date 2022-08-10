#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include <stdio.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <unistd.h>
# include <stdlib.h>
# include <netinet/in.h>
# include <string.h>
# include <iostream>
#include "../request/Request.hpp"
#include "../Config/Parser.hpp"
#include "../response/Response.hpp"
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
# define RECV_SIZE 65536


class Server {
public:
	Server(unsigned int port, std::string host, Config serverConfig);
	long	getFD(void);

	int		setup(void);
	void	setAddr(void);
	long	accept(void);
	void	process(long socket);
	int		recv(long socket);
	int	send(long socket);

	bool is_modify(long socket);
	
private:
	Config								_serverConfig;
	std::map<std::string, std::string>	_cache;
	std::map<long, std::string>			_requests;
	std::map<long, std::string>			_response;
	unsigned int						_port;
	std::string							_host;
	long								_serverFd;
	struct sockaddr_in					_addr;
	Server(void);

};

#endif
