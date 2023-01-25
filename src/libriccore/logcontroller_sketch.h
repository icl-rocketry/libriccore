#include <iostream>
#include <string>
#include <tuple>

//implements globally acessible logger functions in an encapsulated way so that management of the logging objects
//is properly handled in a DI oop way while convienet global logging functions still exist for program logging.

//logging::log<LOGNAME>(logging args);



//loggerbase.h

class loggerBase{
    public:
        virtual void init(){}; // init method here allows assignement of external runtime only resources -> logcontroller.retreive_logger<sys>().init(params);
        
        virtual void log(std::string str)
        {
            std::cout<<str<<std::endl;
        };
        
        virtual void stop(){};
        virtual void start(){};

};

//loggerderived.h
class logger1 : public loggerBase{
    public:
        void log(std::string str,int a){
            std::cout<<"Logger1 : " << str << " " << std::to_string(a) << std::endl;
        }; 
};

class logger2 : public loggerBase{
    public:
        void log(std::string str,float a){
            std::cout<<"Logger1 : " << str << " " << std::to_string(a) << std::endl;
        }; 
};


//logger config header
namespace lgc{
    enum class LOGGER_NAMES{
        sys_logger_1,
        sys_logger_2
    };

    constexpr auto config = std::make_tuple(logger1(),logger2());
};
//






//logcontroller.h

class logcontroller{
    logcontroller(){};
    ~logcontroller(){};

    public:
        logcontroller(logcontroller const&) = delete;
        void operator=(logcontroller const&)  = delete;

    
    private:
        friend class Ilogger;
        friend class Ilogging;

        static constexpr auto config = lgc::config;



        static logcontroller& getInstance(){ 
            static logcontroller lg;
            return lg;
            };


        template<lgc::LOGGER_NAMES name>
        constexpr auto retrieve_logger(){
            return std::get<static_cast<int>(name)>(config);
        }; //decltype tuple return type for return type 
        
        



};

struct Ilogger{
    private:
        friend class system;
        static logcontroller& getLoggerInstance(){
            return logcontroller::getInstance();
        }
};

void test_log(int &p1,std::string p2){
    std::cout<<"p1 address:" <<&p1<<std::endl;
    std::cout<<"p1 value:" <<p1<<std::endl;
}

struct Ilogging{

    template<lgc::LOGGER_NAMES name,class... Ts>
    // check LOGGER_NAMES contains name
    static void log(Ts&&... args){
        std::cout<<std::to_string(static_cast<int>(name))<<" called!"<<std::endl;
        // test_log(std::forward<Ts>(args)...);
        logcontroller::getInstance().retrieve_logger<name>().log(std::forward<Ts>(args)...);
        };


};

// system.h

class system{

    private:
        logcontroller& lc = Ilogger::getLoggerInstance();
};



int main() {
    int a = 10;
    std::cout<<&a<<std::endl;

    Ilogging::log<lgc::LOGGER_NAMES::sys_logger_2>("hi",10.4);
}