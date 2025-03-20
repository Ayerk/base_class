#include <iostream>

#include <string>

using namespace std;

class Log{
public:
    Log(){};
    Log(const Log& log) = delete;//ban copy constructor.
    Log& operator=(const Log& log) = delete;//ban copy assignment operator.

    static Log& GetInstance(){
        if(log == nullptr){
            log = new Log();
        }
        return *log;
    }
    
    void PrintLog(const string& msg){
        cout << __TIME__<<" : "<< msg << endl;
    }
private:
    static Log* log;
};

Log* Log::log = nullptr;

int main(){
    Log::GetInstance().PrintLog("Hello Log!");

    return 0;
}