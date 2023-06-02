#pragma once
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

// Based on this implementation: https://stackoverflow.com/a/32593825/21568848
namespace Utils
{
	class ThreadPool
	{
	private:
		std::vector<std::thread> mThreads;

		std::mutex queueMutex;
		std::queue<std::function<void()>> mJobs;

		std::atomic<int> mJobsActive;

		std::condition_variable activateCondition;

		bool shouldTerminate = false;

		void ThreadLoop();
	public:
		ThreadPool() = default;

		void Start();
		void Start(uint8_t threadCount);

		void QueueJob(const std::function<void()>& job);

		bool Busy();

		void Clear();
	};

	inline void ThreadPool::ThreadLoop()
	{
		while (true) {
			std::function<void()> job;
			{
				std::unique_lock<std::mutex> lock(queueMutex);
				activateCondition.wait(lock, [this] {
					return !mJobs.empty() || shouldTerminate;
					});
				if (shouldTerminate)
					return;

				job = mJobs.front();
				++mJobsActive;
				mJobs.pop();
			}
			job();
			--mJobsActive;
		}
	}

	inline void ThreadPool::Start()
	{
		auto THREADS = static_cast<uint8_t>(std::thread::hardware_concurrency());
		if (THREADS == 0)
		{
			std::cout << "WARNING: AMOUNT OF THREADS UNKNOWN" << std::endl;
			THREADS = 8;
		}

		mThreads.resize(THREADS);

		for (uint8_t i = 0; i < THREADS; i++)
			mThreads.at(i) = std::thread(&ThreadPool::ThreadLoop, this);
	}

	inline void ThreadPool::Start(uint8_t threadCount)
	{
		mThreads.resize(threadCount);
		for (uint8_t i = 0; i < threadCount; i++)
			mThreads.at(i) = std::thread(&ThreadPool::ThreadLoop, this);
	}

	inline void ThreadPool::QueueJob(const std::function<void()>& job)
	{
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			mJobs.push(job);
		}
		activateCondition.notify_one();
	}

	inline bool ThreadPool::Busy()
	{
		bool poolbusy;
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			poolbusy = !mJobs.empty() || mJobsActive!=0;
		}
		return poolbusy;
	}

	inline void ThreadPool::Clear()
	{
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			shouldTerminate = true;
		}
		activateCondition.notify_all();

		for (std::thread& thread : mThreads) {
			thread.join();
		}
		mThreads.clear();
	}
}
