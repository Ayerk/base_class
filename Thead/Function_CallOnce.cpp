#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <vector>


using namespace std;

once_flag flag;
mutex mtx;

class Log{
public:
    Log(){};
    Log(const Log& log) = delete;//ban copy constructor.
    Log& operator=(const Log& log) = delete;//ban copy assignment operator.

    static Log& GetInstance(){
        //call_once can only be used in thread func.
        call_once(flag, Log::init);//call_once ensures that the function is called only once.
        return *log;
    }
    
    static void init(){
        if(log == nullptr){
            log = new Log();
        }
    }

    void PrintLog(const string& msg,int i){
        mtx.lock();
        cout << i <<" --> "<< __TIME__<<" : "<< msg << endl;
        mtx.unlock();
    }
private:
    static Log* log;
};

Log* Log::log = nullptr;


void PrintError(int i){
    Log::GetInstance().PrintLog("Error!",i);
}

int main(){
    vector<thread> threads;
    for(int i = 0; i < 10; i++){
        threads.emplace_back(thread(PrintError,i));//function address, argc1, argc2, ...
    }
    
    for(auto& t : threads){
        t.join();
    }

    return 0;
}