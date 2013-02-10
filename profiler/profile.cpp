/*
 * profile.cpp
 *
 *  Created on: Feb 8, 2013
 *      Author: jordan
 */
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <sys/timerfd.h>
#include <boost/program_options.hpp>

#include <poll.h>

#define _MAX_FD 1024
#define _STR_LEN 1000
#define _POLL_INTERVAL 10
#define _DEFAULT_TIMEOUT 10
#define _OPT_TARGET "--target"

using namespace std;
namespace po = boost::program_options;

int main(int argc, char **argv){

	vector<pid_t> pidList;
	pid_t pid;
	struct pollfd *fdArr;


	// Get the program options
	vector<int> fdList;
	po::options_description desc("Allowed Values");
	long targetNs = -1;
	desc.add_options()
		("help,h", "Display the help")
		("fds", po::value<vector<int> >(&fdList), "File descriptors")
		("target-ns,t", po::value<long>(&targetNs), "The target time to profile"
				"the data.  Anything above this number is a FAIL.");
	po::positional_options_description po_desc;
	po_desc.add("fds", _MAX_FD);

	po::variables_map vMap;
	try{
		po::store(po::command_line_parser(argc, argv).options(desc)
				.positional(po_desc).run(), vMap);
		po::notify(vMap);
	}catch(po::unknown_option &uo){
		cout << desc << endl;
		exit(EXIT_SUCCESS);
	}

	if (vMap.count("help")){
		cout << desc << endl;
		exit(EXIT_SUCCESS);
	}

	if (vMap.count("target-ns")){
		cout << "Nanosecond goal: " << targetNs << " ( or ";
		cout << (targetNs/1000) << " \302\265s )" << endl;
	}

	fdArr = new struct pollfd[fdList.size()];
	for (int i = 0; i < fdList.size(); ++i){
		fdArr[i].fd = fdList[i];
		fdArr[i].events = POLLHUP|POLLNVAL;
		fdArr[i].revents = 0;
	}

	int res;
	while (1){
		res = poll(fdArr, fdList.size(), _DEFAULT_TIMEOUT);
		if (res == 0){
			cout << "No FDs were ready." << endl;
			exit(EXIT_FAILURE);
		}else if (res < 0){
			perror("Error polling: ");
			exit(EXIT_FAILURE);
		}else{
			cout << "Polling..." << endl;
		}
	}
}

