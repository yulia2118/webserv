#include "WebServer.hpp"

void		Server::setAddr(void)
{
	_addr.sin_family = AF_INET;
	if (_host == "")
		_addr.sin_addr.s_addr = INADDR_ANY;
	else
		_addr.sin_addr.s_addr = inet_addr(_host.c_str());
	_addr.sin_port = htons(_port);
	memset(_addr.sin_zero, '\0', sizeof _addr.sin_zero);
}

Server::Server(unsigned int port, std::string host, Config serverConfig)
{
	_serverConfig = serverConfig;
	this->_port = port;
	this->_host = host;
	_serverFd = -1;
}

int		Server::setup(void)
{
	int yes = 1;
	if ((_serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("In socket");
		exit(EXIT_FAILURE);
	}
	setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	if (_serverFd == -1)
	{
		std::cerr <<  "Could not create server."  << std::endl;
		return (-1);
	}
	this->setAddr();
	if (bind(_serverFd, (struct sockaddr *)&_addr, sizeof(_addr))<0)
	{
		perror("In bind");
		exit(EXIT_FAILURE);
	}
	if (listen(_serverFd, 10) < 0)
	{
		perror("In listen");
		exit(EXIT_FAILURE);
	}
	return (0);
}


long Server::getFD()
{
	return (_serverFd);
}

long		Server::accept(void)
{
	long	new_socket;
	int addrlen = sizeof(_addr);

	new_socket = ::accept(_serverFd, (struct sockaddr *)&_addr, (socklen_t*)&addrlen);
	if (new_socket == -1) 
		perror("accept");
	else 
	{
		fcntl(new_socket, F_SETFL, O_NONBLOCK);
		_requests.insert(std::make_pair(new_socket, ""));
	}
	return (new_socket);
}

void		Server::process(long socket)
{
	std::cout << "\033[33m" << _cache.count(_requests[socket]) << "\033[0m" << std::endl;
	if (_requests[socket].size() < 1000)
			std::cout << "\nRequest :" << std::endl << "[" << _requests[socket]  << "]" << std::endl;
		else
			std::cout << "\nRequest :" << std::endl << "["  << _requests[socket].substr(0, 1000) << "..." << _requests[socket].substr(_requests[socket].size() - 10, 15) << "]" << std::endl;
	if ((_cache.count(_requests[socket]) != 0) && is_modify(socket))
		_cache.erase(_requests[socket]);
	if (_cache.count(_requests[socket]) == 0)
	{
		std::string response;
		std::string str = _requests[socket].c_str();
		char *cstr = new char[str.length() + 1];
		strcpy(cstr, str.c_str());
		Request req(cstr, &_serverConfig);
		Response res(req);
		if (_requests[socket] != "")
		{
			response = res.getResponse();
			if (res.getMethod() == "GET")
				_cache[_requests[socket]] = response;
			_response.insert(std::make_pair(socket, response));
			_requests.erase(socket);
		}
		delete [] cstr;
	}
	else
	{
		_response.insert(std::make_pair(socket, _cache[_requests[socket]]));
		_requests.erase(socket);
	}
}

bool Server::is_modify(long socket)
{
	std::string str = _requests[socket].c_str();
	char *cstr = new char[str.length() + 1];
	strcpy(cstr, str.c_str());
	Request req(cstr, &_serverConfig);

	std::string find_resp = _cache[_requests[socket]];
	
	int i = find_resp.find("Last-Modified:");
	
	if (i != -1)
	{
		std::string cache_modif_date = find_resp.substr(i + 15, find_resp.find("\r\n", i) - (i + 15));

		Response resp_for_date(req);
		std::string last_modif = resp_for_date.getLastModif();
		delete [] cstr;
		if (cache_modif_date == last_modif)
			return false;
		else
			return true;
	}
	delete [] cstr;
	return false;
}

int			Server::recv(long socket)
{
	int nbytes;
	std::vector<char> request(RECV_SIZE);
	if ((nbytes = ::recv(socket, request.data(), request.size(), 0)) <= 0) 
	{
		if (nbytes == 0) 
			std::cout << "\rConnection closed by client, socket number" << socket << std::endl;
		else if (nbytes == -1)
			perror("\r recv");
		::close(socket);
		return (-1);
	}
	_requests[socket] += request.data();
	return (1);
}

int			Server::send(long socket)
{
	static std::map<long, size_t>	sent_data;
	if (sent_data.find(socket) == sent_data.end())
		sent_data[socket] = 0;

	if (_response[socket].size() < 1000 && sent_data[socket] == 0)
		std::cout << "\rResponse :                " << std::endl << "[" << _response[socket] << "]\n" << std::endl;
	else
		std::cout << "\rResponse :                " << std::endl << "["  << _response[socket].substr(0, 1000) << "..." << _response[socket].substr(_response[socket].size() - 10, 15) << "]\n" << std::endl;

	std::string s = _response[socket].substr(sent_data[socket], RECV_SIZE);
	int rVal = ::send(socket, s.c_str(), s.size(), 0);

	if (rVal == -1)
	{
		::close(socket);
		sent_data[socket] = 0;
		perror("send");
		return (-1);
	}
	else
	{
		sent_data[socket] += rVal;
		if (sent_data[socket] >= _response[socket].size())
		{
			_response.erase(socket);
			sent_data[socket] = 0;
			return (0);
		}
		return (1);
	}
}
