#pragma once
#include <functional>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

namespace concurrent {
namespace wrapper {

class threadWrapper {
	thread th;

public:
    //Delete the copy constructor
    threadWrapper(const threadWrapper&) = delete;
    //Delete the Assignment operator
    threadWrapper& operator=(const threadWrapper&) = delete;
	
	//Parameterized Constructor
	threadWrapper(thread _t): 
		th(move(_t)) {}
		// if (!th.joinable()){
      	// 		throw logic_error("No thread!");
		// }
    

	// Move Constructor
	threadWrapper(threadWrapper && obj): 
		th(move(obj.th)) 
	{}

	//Move Assignment Operator
	threadWrapper & operator=(threadWrapper && obj) {
	    if (th.joinable()) {
	        th.join();
	    }
	    th = move(obj.th);
	    return *this;
	}

	// Destructor
	~threadWrapper() {
	    if (th.joinable()) {
	        th.join();
	    }
	}	
}; /* class threadWrapper */


}
}
