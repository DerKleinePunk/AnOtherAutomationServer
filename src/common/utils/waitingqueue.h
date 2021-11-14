//based on https://github.com/vichargrave/wqueue/blob/master/wqueue.h
//https://codereview.stackexchange.com/questions/49820/thread-safe-queue
//https://stackoverflow.com/questions/12805041/c-equivalent-to-javas-blockingqueue/12805690#12805690
//https://gist.github.com/thelinked/6997598
//https://github.com/bhhbazinga/BlockingQueue

#pragma once

#include <mutex>
#include <deque>
#include <condition_variable>

//Todo Understand make it working
//template <typename T, Container = std::deque<T>> 
template <typename T> 
class WaitingQueue
{
	std::mutex _mutex;
	std::condition_variable _singnal;
    std::deque<T>           _queue;//Container

public:
	WaitingQueue() 
	{

	} 

	~WaitingQueue() {
		_queue.clear();
	}
    
    void clear() 
	{
        std::unique_lock<std::mutex> lock(_mutex);
        _queue.clear();
    }

	void add(T const& item) 
	{
		{
			std::unique_lock<std::mutex> lock(_mutex);
			_queue.push_back(item);
		}
		_singnal.notify_one();
	}

	T waitAndRemove() {
		std::unique_lock<std::mutex> lock(_mutex);
		if (_queue.empty()) {
			_singnal.wait(lock, [this] { return !_queue->empty(); });
		}
		T item(std::move(_queue.front()));
		_queue.pop_front();
		return item;
	}

	bool tryRemove(T& item, std::chrono::milliseconds duration) {
		std::unique_lock<std::mutex> lock(_mutex);
		if (_queue.empty()) {
			if(!_singnal.wait_for(lock, duration, [this] { return !_queue.empty(); })) {
				return false;
			}
		}
		item = std::move(_queue.front());
		_queue.pop_front();
		return true;
	}

	int size() {
		std::unique_lock<std::mutex> lock(_mutex);
		int size = _queue.size();
		return size;
	}
};