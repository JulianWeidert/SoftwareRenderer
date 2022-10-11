#pragma once

#include <array>
#include <queue>

#include <functional>

#include <thread>
#include <mutex>
#include <condition_variable>

namespace sr {

	template<size_t count>
	class ThreadPool {
	private:
		std::array<std::thread, count> threads;
		std::array<volatile bool, count> threadStatus = {0}; // indicates if a thread is working
		
		std::queue<std::function<void()>> jobs;

		std::mutex queueLock;
		std::condition_variable resumeCondition;

		bool terminated = false;
		bool active = true;

		void ThreadProc(int id);

	public:

		ThreadPool();
		~ThreadPool();

		ThreadPool(const ThreadPool& pool) = delete;

		ThreadPool& operator=(const ThreadPool& pool) = delete;

		void addJob(const std::function<void()>& job);

		void activate();
		void deactivate();

		bool isBusy() const;

	};


	template<size_t count>
	void ThreadPool<count>::ThreadProc(int id) {
		while (true) {
			std::function<void()> job;
			{
				std::unique_lock<std::mutex> lock(this->queueLock);
				this->resumeCondition.wait(lock, [this] {
					return (!this->jobs.empty() && active) || this->terminated;
					});

				if (this->terminated) {
					return;
				}

				threadStatus[id] = true;
				job = jobs.front();
				jobs.pop();
			}
			job();
			threadStatus[id] = false;
		}
	}

	template<size_t count>
	ThreadPool<count>::ThreadPool() {
		for (int i = 0; i < count; ++i) this->threads[i] = std::thread(&ThreadPool<count>::ThreadProc, this, i);
	}

	template<size_t count>
	ThreadPool<count>::~ThreadPool() {
		this->terminated = true;
		this->resumeCondition.notify_all();
		for (auto& t : this->threads) t.join();
	}

	template<size_t count>
	void ThreadPool<count>::addJob(const std::function<void()>& job) {
		{
			std::unique_lock lock(this->queueLock);
			this->jobs.push(job);
		}
		resumeCondition.notify_one();
	}

	template<size_t count>
	void ThreadPool<count>::activate() {
		this->active = true;
	}

	template<size_t count>
	void ThreadPool<count>::deactivate() {
		this->active = false;
	}

	template<size_t count>
	bool ThreadPool<count>::isBusy() const {
		bool busy = !this->jobs.empty();
		for (auto status : this->threadStatus) busy |= status;
		return busy;
	}

}
