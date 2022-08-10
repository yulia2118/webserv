#include "server/Servers.hpp"

int		main(int ac, char **av)
{
	AllServers	servers;
	try {
		if (ac > 1)
			servers.config(av[1]);
		else
			std::cerr << "ERROR: Config not found" << std::endl;
		if (servers.setup() == -1)
			return (1);
		servers.run();
	}
	catch (const char *str)
	{
		std::cerr << "ERROR: " << str << std::endl;
		return (1);
	}
	return (0);
}
