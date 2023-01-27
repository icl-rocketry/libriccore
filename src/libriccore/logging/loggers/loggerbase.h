#pragma once

class LoggerBase{
    public:

        LoggerBase():
        initalized(false)
        {};
        
        ~LoggerBase(){};


        virtual void init(){};
        virtual void start(){};
        virtual void stop(){};
        
    
    private:
        bool initalized;

};