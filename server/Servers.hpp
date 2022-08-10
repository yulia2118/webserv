#ifndef SERVERS_HPP
#define SERVERS_HPP


#include "WebServer.hpp"

class AllServers {
	
public:
	AllServers(void) {};
	void	config();
	void	config(std::string file);
	int		setup(void);
	void	run(void);

private:
	Parser						_config;
	std::map<long, Server>		_servers;
	std::map <long, Server *>	_accepted_sockets;
	std::vector<int>			_ready;
	fd_set						_masterFD;
	fd_set 						_read_fds;
	fd_set 						_write_fds;
	long						_fdMax;
	int							select();
	void						send();
	void						recvv();
	void						acceptt();
};

#endif
