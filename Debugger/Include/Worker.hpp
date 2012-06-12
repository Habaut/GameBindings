#pragma once

#include "pthread.h"
#include <iostream>

class Worker
{
private:
	pthread_t thread;
	std::istream& input;
	std::ostream& output;
    std::string debug_name;
    std::ostream& debug_log;
	//pthread_mutex record_mutex;

	static void* callback(void* obj);
	volatile bool stop;
	volatile bool is_running;
	void run_loop();

public:
	Worker(std::istream& input_, std::ostream& output_, std::string debug_name_, std::ostream& debug_log_);
	~Worker();
	void stop_thread();
	bool is_stopped() const;
};
