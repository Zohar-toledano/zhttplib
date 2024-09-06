#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <functional>

template <typename TaskArg>
class ThreadPool
{
public:
	ThreadPool(std::function<void(TaskArg)> th) : taskHandler(th) {}
	ThreadPool(){}

	void Start()
	{
		const uint32_t num_threads = std::thread::hardware_concurrency(); // Max # of threads the system supports
		for (uint32_t ii = 0; ii < num_threads; ++ii)
		{
			threads.emplace_back(std::thread(&ThreadPool::ThreadLoop, this));
		}
	}
	void QueueTask(TaskArg task)
	{
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			tasks.push(task);
		}
		mutex_condition.notify_one();
	}
	void Stop()
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
	bool busy()
	{
		bool poolbusy;
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			poolbusy = !tasks.empty();
		}
		return poolbusy;
	}

private:
	void ThreadLoop()
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
	std::function<void(TaskArg)> taskHandler;
	bool should_terminate = false;			 // Tells threads to stop looking for jobs
	std::mutex queue_mutex;					 // Prevents data races to the job queue
	std::condition_variable mutex_condition; // Allows threads to wait on new jobs or termination
	std::vector<std::thread> threads;
	std::queue<TaskArg> tasks;
};
