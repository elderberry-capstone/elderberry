#include <cstdio>
#include <libusb-1.0/libusb.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
using namespace std;
namespace ba = boost::asio;
namespace bru = boost::random;

#define ERROR cerr << "\033[31;1mERROR:\t"
#define WARN cerr << "\033[33mWARNING:\t"
#define INFO cout << "\033[36mINFO:\t"
#define CRITICAL cerr << "\033[5;41;37mCRITICAL:\t"
#define DEBUG cout << "\033[37mDEBUG:\t"
#define ENDL "\033[0m" << endl

const unsigned int MY_TIMEOUT = 10000;  // Default timeout.
const unsigned int _ARGC = 3;    // Needed number of arguments.

const short PORT_I = 2; // Port index for argv.
const short ADDR_I = 1; // Address index for argv.
const int MIN_CHARS = 0;
const int MAX_CHARS = 30;
const int MIN_CHAR_RANGE = 32;
const int MAX_CHAR_RANGE = 126;

const char *gen_buf(){
	boost::random::mt19937 gen;
	bru::uniform_int_distribution<> dist1(MIN_CHARS, MAX_CHARS);
	bru::uniform_int_distribution<> dist2(MIN_CHAR_RANGE, MAX_CHAR_RANGE);

	int numChars = dist1(gen);
	string str = NULL;
	for (int i = 0; i < numChars; ++i){
		str = str + (char) dist2(gen);
	}

	INFO << "Generated \"" << str << "\"" << ENDL;
	return str.c_str();
}

int main(int argc, char **argv){
    if (argc != _ARGC){
        ERROR << "You provided " << argc << " arguments. Expected " <<
            _ARGC << "." << ENDL;
        exit(EXIT_FAILURE);
    }
    
    auto portNumber = atoi(argv[PORT_I]);
    auto addr = ba::ip::address::from_string(argv[ADDR_I]);
    

    auto endpoint = ba::ip::basic_endpoint<const ba::ip::address *>(
    		&addr, portNumber );
    ba::io_service ioserv;
    auto sock = ba::basic_socket<int, int>(endpoint, ioserv);
    ba::write(sock, ba::buffer(gen_buf(), BUFSIZ));
}
