
#pragma once
#include <string>


/*pass logger objects into log controller
logcontroller.get(enum/string) -> void log(std::string_view) func -> cant do this as all loggers would need to implement the same log type or some sort of deriving
logcontroller::get<logger_type>.log() -> requies only one type of logger class in the log controller
*/

struct telemetry_struct;

enum logfiles{
    syslog = 0,
    tellog
};



class logger{
    public:
        logger(std::string file_path);
        void start();
        void stop();

};

class syslog : public logger{
    public:
        void log(std::string);
};

class tellog: public logger{
    public:
        void log(telemetry_struct);
};


auto logfile_defintion = {{logfiles::syslog,{syslog,"./system_log.txt"}}};


template<class... Ts>
class logcontroller{
    public:
        logcontroller();
        
        
        static void get();
        static void add_log_files();


};