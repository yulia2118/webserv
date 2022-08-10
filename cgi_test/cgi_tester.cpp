#include <iostream>
#include <stdlib.h>

const std::string ENV[ 3 ] = {
		"REQUEST_METHOD", "SERVER_PROTOCOL", "PATH_INFO" };

int main () {
	std::cout << "Content-type:text/html\r\n\r\n";
	std::cout << "<html>\n";
	std::cout << "<head>\n";
	std::cout << "<title>CGI Environment Variables</title>\n";
	std::cout << "</head>\n";
	std::cout << "<body>\n";
	std::cout << "<table border = \"0\" cellspacing = \"2\">";

	for ( int i = 0; i < 3; i++ ) {
		std::cout << "<tr><td>" << ENV[ i ] << "</td><td>";

		// attempt to retrieve value of environment variable
		char *value = getenv( ENV[ i ].c_str() );
		if ( value != 0 ) {
			std::cout << value;
		} else {
			std::cout << "Environment variable does not exist.";
		}
		std::cout << "</td></tr>\n";
	}

	std::cout << "</table><\n";
	std::cout << "</body>\n";
	std::cout << "</html>\n";

	return 0;
}
