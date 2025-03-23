#include<iostream>
#include<thread>
#include<mutex>
#include<string>
#include<condition_variable>
#include<queue>

using namespace std;

const int MAX_SIZE = 10;
queue<int> g_queue;
mutex mtx;
condition_variable cv_producer, cv_consumer;


void Producer(){
    for(int i = 0; i < 1000; i++){
        {        
            unique_lock<mutex> lock(mtx);
            cv_producer.wait(lock,[&]{return g_queue.size() < MAX_SIZE;});
            g_queue.push(i);
            cout << "Producer: " << i << endl;
        }
        cv_consumer.notify_one();
        // this_thread::sleep_for(chrono::milliseconds(1));
    }
}

void Consumer(){
    for(int i = 0; i < 1000; i++){
        {
            unique_lock<mutex> lock(mtx);
            cv_consumer.wait(lock,[&]{return !g_queue.empty();});
            int data = g_queue.front();
            g_queue.pop();
            cout << "Consumer: " << data << endl;
        }
        cv_producer.notify_one();
    }
}

int main(){
    thread t1(Producer);
    thread t2(Consumer);

    t1.join();
    t2.join();

    return 0;
}
