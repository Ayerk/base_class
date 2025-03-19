#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

//Learn how to use mutex, lock_guard, unique_lock, timed_mutex.

using namespace std;

int shared_data=0;
mutex mtx;
timed_mutex tmtx;


//lock_guard<mutex> lock(mtx); //lock_guard is a class that manages a mutex object.(RAII)
void foo(){
    for(int i = 0; i < 1000000; i++){
        lock_guard<mutex> lock(mtx);
        shared_data++;    
    }
}

//unique_lock is more flexible than lock_guard. It can be unlocked and locked again.
void foo1(){
    for(int i = 0; i < 2; i++){
        unique_lock<timed_mutex> lock(tmtx,defer_lock);//defer_lock means delay the lock.
        //try_lock() It tries to lock the mutex immediately. If it can lock, it returns true. Otherwise, it returns false.
        if(lock.try_lock_for(chrono::seconds(5))){//wait for 5 seconds to get the lock.(return bool)
            this_thread::sleep_for(chrono::seconds(7));
            shared_data++;
        }
        else{
            cout << "Timeout" << endl;
        } 
    }
}

int main(){

    // thread t1(foo);
    // thread t2(foo);

    thread t1(foo1);
    thread t2(foo1);//task start when thread created.

    t1.join();
    t2.join();//wait for the task to finish.
    
    cout << shared_data << endl;
    return 0;
}
