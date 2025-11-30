#include<iostream>
#include<cstddef>
#include<deque>
#include<mutex>
#include<condition_variable>

template<typename T>
class blocking_queue
{

public:
    
    blocking_queue(size_t size);
    blocking_queue();
    bool push(T t);
    T pop();
    size_t count();

private:
    std::deque<T> m_queue;
    size_t m_size;

    std::mutex m_mx;

    std::condition_variable m_produce;
    std::condition_variable m_consume;
};