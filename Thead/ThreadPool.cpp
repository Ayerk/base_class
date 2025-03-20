#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <queue>
#include <functional>
#include <condition_variable>
#include <ctime>

using namespace std;
//adding a instance pattern to ThreadPool.

static once_flag flag;
mutex mtx;

class ThreadPool{
public:
    ThreadPool() = delete;
    // ThreadPool& operator=(const ThreadPool& pool) = delete;
    ThreadPool(const ThreadPool& pool) = delete;

    ~ThreadPool(){
        {
            unique_lock<mutex> lock(mtx);
            stop = true;
        }
        cv.notify_all();
        for(auto& t : threads){//thread can't be copied.
            t.join();
        }
    }

    template<class F, class... Args>
    void enqueue(F &&f, Args && ...args){
        function<void()> task = bind(forward<F>(f), forward<Args>(args)...);
        {
            unique_lock<mutex> lock(mtx);
            tasks.emplace(move(task));
        }
        cv.notify_one();
    }

    static ThreadPool& GetInstance(int numThreads){
        call_once(flag, ThreadPool::init, numThreads);
        return *pool;
    }

private:
    vector<thread> threads;
    queue<function<void()>> tasks;

    mutex mtx;
    condition_variable cv;

    static ThreadPool* pool;
    bool stop;

    static void init(int numThreads){
        if(pool == nullptr){
            pool = new ThreadPool(numThreads);
        }
    }

    ThreadPool(int numThreads):stop(false){
        for(int i = 0; i < numThreads; i++){
            threads.emplace_back([this]{
                for(;;){
                    function<void()> task;
                    {
                        unique_lock<mutex> lock(mtx);
                        cv.wait(lock,[this]{return stop || !tasks.empty();});
                        if(stop && tasks.empty()){
                            return;
                        }
                        task = move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

};

ThreadPool* ThreadPool::pool = nullptr;
mutex mtx_io;

#include <iostream>
#include <chrono>
#include <ctime>

// 封装在 GetTime 函数中
string GetTime() {
    // 使用 chrono 获取当前时间点
    auto now = chrono::system_clock::now();
    
    // 将时间点转换为 time_t
    time_t now_time_t = chrono::system_clock::to_time_t(now);
    
    // 将 time_t 转换为本地时间
    tm* local_time = localtime(&now_time_t);
    
    // 格式化时间为字符串
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", local_time);
    
    return string(buffer);
}



int main(){
    ThreadPool& tp = ThreadPool::GetInstance(4);
    // cout<<__TIME__<<endl;
    cout<<GetTime()<<endl;
    for(int i = 0; i < 20; i++){
        tp.enqueue([](int i){

            {
                unique_lock<mutex> lock(mtx_io);
                cout << "task : " << i << " start."<<endl;
            }

            this_thread::sleep_for(chrono::seconds(1));

            {
                unique_lock<mutex> lock(mtx_io);
                cout << "task : " << i << " end."<<endl;
            }

        },i);
    }
    tp.~ThreadPool();
    cout<<GetTime()<<endl;
    return 0;
}