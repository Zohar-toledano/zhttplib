#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <functional>

template <typename TaskArg>
class ThreadPool
{
public:
	ThreadPool(std::function<void(TaskArg)> th);
	ThreadPool()=default;

	void Start();
	void QueueTask(TaskArg task);
	void Stop();
	bool busy();
private:
	void ThreadLoop();
	std::function<void(TaskArg)> taskHandler;
	bool should_terminate = false;			 // Tells threads to stop looking for jobs
	std::mutex queue_mutex;					 // Prevents data races to the job queue
	std::condition_variable mutex_condition; // Allows threads to wait on new jobs or termination
	std::vector<std::thread> threads;
	std::queue<TaskArg> tasks;
};



template <typename TaskArg>
ThreadPool<TaskArg>::ThreadPool(std::function<void(TaskArg)> th) : taskHandler(th) {}


template <typename TaskArg>
void ThreadPool<TaskArg>::Start()
{
	const uint32_t num_threads = std::thread::hardware_concurrency(); // Max # of threads the system supports
	for (uint32_t ii = 0; ii < num_threads; ++ii)
	{
		threads.emplace_back(std::thread(&ThreadPool::ThreadLoop, this));
	}
}
template <typename TaskArg>
void ThreadPool<TaskArg>::QueueTask(TaskArg task)
{
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		tasks.push(task);
	}
	mutex_condition.notify_one();
}

template <typename TaskArg>
void ThreadPool<TaskArg>::Stop()
{
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		should_terminate = true;
	}
	mutex_condition.notify_all();
	for (std::thread &active_thread : threads)
	{
		active_thread.join();
	}
	threads.clear();
}

template <typename TaskArg>
bool ThreadPool<TaskArg>::busy()
{
	bool poolbusy;
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		poolbusy = !tasks.empty();
	}
	return poolbusy;
}

template <typename TaskArg>
void ThreadPool<TaskArg>::ThreadLoop()
{
	while (true)
	{
		TaskArg task;
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			mutex_condition.wait(lock, [this]
								 { return !tasks.empty() || should_terminate; });
			if (should_terminate)
			{
				return;
			}
			task = tasks.front();
			tasks.pop();
		}
		taskHandler(task);
	}
}