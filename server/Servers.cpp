 #include "Servers.hpp"

void	AllServers::config(std::string argv)
{
	_config.parser(argv);
}

int		AllServers::setup(void)
{
	FD_ZERO(&_masterFD);
	_fdMax = 0;

	for (unsigned long i = 0; i < _config.getServers().size(); i++)
	{
		uint16_t port = _config.getServers()[i].getListenPort();
		std::string host_string = _config.getServers()[i].getListenIp();
		Server		serv(port, host_string, _config.getServers()[i]);
		long		fd;

		if (serv.setup() != -1)
		{
			fd = serv.getFD();
			FD_SET(fd, &_masterFD);
			_servers.insert(std::make_pair(fd, serv));
			if (fd > _fdMax)
				_fdMax = fd;
			std::cout << "Setting up " << host_string << ":" << port << std::endl;
		}
	}
	if (_fdMax == 0)
	{
		std::cout << "Could not setup cluster !" << std::endl;
		FD_ZERO(&_masterFD);
		return (-1);
	}
	else
		return (0);
}

int		AllServers::select()
{
		struct timeval tv;
		tv.tv_sec  = 1;
		tv.tv_usec = 0;
		_read_fds = _masterFD;
		FD_ZERO(&_write_fds);
		for (std::vector<int>::iterator it = _ready.begin() ; it != _ready.end() ; it++)
			FD_SET(*it, &_write_fds);
		std::cout << "\rWaiting on a connection " <<  std::flush;
		return (::select(_fdMax + 1, &_read_fds, &_write_fds, NULL, &tv));
}

void	AllServers::send()
{
	for (std::vector<int>::iterator it = _ready.begin(); it != _ready.end(); it++)
	{
		if (FD_ISSET(*it, &_write_fds))
		{
			long rVal = _accepted_sockets[*it]->send(*it);
			_ready.erase(it);
			if (rVal == -1)
			{
				_accepted_sockets.erase(*it);
				_ready.erase(it);
				FD_CLR(*it, &_masterFD);
				FD_CLR(*it, &_read_fds);
			}
			break;
		}
	}
}

void	AllServers::recvv()
{
	int rVal;

	for (std::map<long, Server *>::iterator it = _accepted_sockets.begin(); it != _accepted_sockets.end() ; it++)
	{
		long accepted_socket = it->first;
		if (FD_ISSET(accepted_socket, &_read_fds))
		{
			rVal = it->second->recv(accepted_socket);
			if (rVal == 1)
			{
				it->second->process(accepted_socket);
				_ready.push_back(accepted_socket);
			}
			else
			{
				FD_CLR(accepted_socket, &_read_fds);
				FD_CLR(accepted_socket, &_masterFD);
				_accepted_sockets.erase(accepted_socket);
				it = _accepted_sockets.begin();
			}
			break;
		}
	}
}

void	AllServers::acceptt()
{
	for (std::map<long, Server >::iterator it = _servers.begin(); it != _servers.end() ; it++)
	{
		long serv_socket = it->first;
		if (FD_ISSET(serv_socket, &_read_fds))
		{
			long accepted_socket = it->second.accept();
			if (accepted_socket != -1)
			{
				FD_SET(accepted_socket, &_masterFD);
				_accepted_sockets.insert(std::make_pair(accepted_socket, &(it->second)));
				if (accepted_socket > _fdMax)
					_fdMax = accepted_socket;
			}
			break;
		}
	}
}


void	AllServers::run(void)
{
	int rVal;

	while (1)
	{
		rVal = select();
		if (rVal == 0)
			continue;
		else if (rVal > 0)
		{
			send();
			std::cout << "\r-Received a connection-!   " << std::flush;
			recvv();
			acceptt();
		}
		else if (rVal == -1)
		{
			perror("select");
			for (std::map<long, Server *>::iterator it = _accepted_sockets.begin(); it != _accepted_sockets.end() ; it++)
				::close(it->first);
			_accepted_sockets.clear();
			_ready.clear();
			FD_ZERO(&_masterFD);
			for (std::map<long, Server>::iterator it = _servers.begin() ; it != _servers.end() ; it++)
				FD_SET(it->first, &_masterFD);
		}
	}
}
