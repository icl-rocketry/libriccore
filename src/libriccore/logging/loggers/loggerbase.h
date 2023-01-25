#pragma once

class LoggerBase{
    public:

        constexpr LoggerBase():
        initalized(false)
        {};
        
        ~LoggerBase(){};


        virtual void init(){};
        virtual void start(){};
        virtual void stop(){};
        
    
    private:
        bool initalized;

};