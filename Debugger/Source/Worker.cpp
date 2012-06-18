#include "Worker.hpp"

using namespace std;
using namespace Iocaste::Debugger;

Worker::Worker(AbstractInput<string>& input_, AbstractOutput<string>& output_, string debug_name_, std::ostream& debug_log_)
    :
        input(input_), output(output_),
        debug_name(debug_name_), debug_log(debug_log_),
        stop(false), is_running(false)
{
	is_running = true;
	//pthread_mutex_init(&record_mutex, NULL);
	pthread_create(&thread, NULL, Worker::callback, this);
}

Worker::~Worker()
{
	if (is_running)
		stop_thread();
}

void Worker::stop_thread()
{
	stop = true;
	pthread_join(thread, NULL);
}

void* Worker::callback(void* obj)
{
	static_cast<Worker*>(obj)->run_loop();
	return NULL;
}

void Worker::run_loop()
{
    std::string line;

    while (!stop)
    {
        if (input.HasData())
        {
            string s = input.ReadData();
            debug_log << s << std::flush;
            output.WriteData(s);
        }
    }

    std::cerr << "Worker " << debug_name << " finished" << std::endl;
    is_running = false;
}

bool Worker::is_stopped() const
{
    return !is_running;
}
