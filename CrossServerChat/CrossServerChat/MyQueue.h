#pragma once
#include <queue>
#include <future>
template <typename T>
class MyQueue
{
public:
	inline MyQueue() {
	}
	inline ~MyQueue()
	{
		_IsLoad = false;
		_task_cv.notify_all(); // 唤醒所有线程执行
	}
	void push(const T value)
	{
		std::lock_guard<std::mutex> lock{ _lock };
		myQueue.emplace(value);
		_task_cv.notify_one();
	}
	T pop()
	{
		std::unique_lock<std::mutex> lock{ _lock };
		if (myQueue.empty())
			return "";
		return move(myQueue.front());
	}
	T popWait()
	{
		std::unique_lock<std::mutex> lock{ _lock };
		_task_cv.wait(lock, [this] {
			return !_IsLoad || !myQueue.empty();
		});
		if (!_IsLoad || myQueue.empty())
			return T();
		T value = myQueue.front();
		myQueue.pop();
		return value;
	}
	void notify()
	{
		_IsLoad = false;
		_task_cv.notify_all();
	}
private:
	std::queue<T> myQueue;
	std::atomic<bool> _IsLoad{ true };
	std::mutex _lock;
	std::condition_variable _task_cv;
};