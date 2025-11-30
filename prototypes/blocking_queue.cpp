#include "blocking_queue.h"
#include<thread>

template<typename T>
blocking_queue<T>::blocking_queue()
{
}

template<typename T>
blocking_queue<T>::blocking_queue(size_t size)
{
    m_size = size;
}

template<typename T>
bool blocking_queue<T>::push(T t)
{
    std::unique_lock lck {m_mx};

    while(true)
    {
        if(m_queue.size() < m_size)
            break;
        
        m_produce.wait(lck);
    }

    m_queue.push_back(t);
    m_consume.notify_all();

    return true;
}

template<typename T>
T blocking_queue<T>::pop()
{
    std::unique_lock lck {m_mx};

    while(true)
    {
        if(m_queue.size() > 0)
            break;
        
        m_consume.wait(lck);
    }

    T v = m_queue.front();
    m_queue.pop_front();
    m_produce.notify_all();

    return v;
}

template<typename T>
size_t blocking_queue<T>::count()
{
    std::unique_lock lck(m_mx);
    size_t size = m_queue.size();
    lck.unlock();

    return size;
}

int main(int argc, char const *argv[])
{
    blocking_queue<int>* bq = new blocking_queue<int>(5);

    std::thread t1 { [&]()
        {
            for(int i = 0; i < 100; i++)
            {
                auto bSuccess = bq->push(i);
                std::cout << "The insert in blocking queue of element: " << i << ", Total Elements: " << bq->count() << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    };

    std::thread t2 { [&]()
        {
            for(int i = 0; i < 100; i++)
            {
                int v = bq->pop();
                std::cout << "The pop from blocking queue of element: " << v << ", Total Elements:" << bq->count() << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        }
    };

    t1.join();
    t2.join();

    return 0;
}
