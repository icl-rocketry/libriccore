
#pragma once

#include <cstdint>
#include <memory>

#include "riccoretypes.h"


class State
{

  public:

    State(RicCoreTypes::stateID_t ID):
    stateID(ID)
    {
      // time_entered_state = millis();
        // _sm->systemstatus.newFlag(_curr_stateID,"state entered");
    };
    
    virtual ~State(){
       //   time_exited_state = millis();
        // time_duration_state = time_exited_state - time_entered_state;
        //  _sm->systemstatus.deleteFlag(_curr_stateID,"state exited | state duration: " + std::to_string(time_duration_state));

    };


    virtual std::unique_ptr<State> update(){
      return std::unique_ptr<State>(nullptr);
    };


     RicCoreTypes::stateID_t getID(){
      return stateID;
     };

  protected:
    const RicCoreTypes::stateID_t stateID;

    uint64_t time_entered_state;
    uint64_t time_exited_state;
    uint64_t time_duration_state;

};
