#ifndef HEADING_CONTROLLER_H
#define HEADING_CONTROLLER_H SEP_2014

#include "Control_vec.h"

#include <arduino.h>

#include <compass.h>
#include <compass_fb.h>
#include <compass_msg.h>

#include <Rover_plant.h>
#include <Rover_plant_fb.h>
#include <Rover_plant_msg.h>

#include <Vector.h>

#include <Controller.h>
#include <Direction.h>
#include <Feedback.h>
#include <Message.h>
#include <Motor.h>
#include <Plant.h>
#include <Sensor.h>

#define DEBUG_RUN 0
#define DEBUG_CONTROL 0
#define DEBUG_MESSAGE 0

//************************************************************************
//*                         WRAP DIRECTION
//************************************************************************
namespace Wrap {
	enum dir {up, down};
}

//*******************************************************************
//*                         HEADING_CONTROLLER
//*******************************************************************

class Heading_controller : public Controller {
private:
  //From the base class
  //    Vector<Sensor*> sensors;
  //    Vector<Plant*> plants;    
    
  //Sensor 1 Feedback Handle
  Compass_fb<Compass_msg>* c_fb;
  Compass_msg c_msg;
    
  //Plant1 Feedback Handle
  Rover_plant_fb<Rover_plant_msg>* p_fb;
  Rover_plant_msg p_msg;
     
  //Active control_vec is the current motor command
  Control_vec active_control_vec;
  		    
  void run_sensors();       //PURE VIRTUAL FROM BASE
  
  void calc_plant_inputs(); //PURE VIRTUAL FROM BASE
  
  void update_plant_msgs(); //PURE VIRTUAL FROM BASE
  
  // DATA UNIQUE TO THIS CONTROLLER
  int target;		//heading the controller is driving toward
  int upper_lim;
  int lower_lim;
  int tol;	        //value plus or minus target that results in no control effort
  
  // HELPER FUNCTIONS UNIQUE TO THIS CONTROLLER
  Control_vec set_control_vec();
  void set_limits(const int center_heading);
  int wrap(int heading, int tolerance, Wrap::dir wd);
  int delta(int current_Z);		//return +/- of current head from target head
 
public:
    Heading_controller(int number_of_sensors, 
		int number_of_plants, 
		int _target_heading,
		int _tolerance);

    bool attach(Sensor* s); //PURE VIRTUAL FROM BASE
  
    bool attach(Plant* p); //PURE VIRTUAL FROM BASE
  
    void update_state();   //PURE VIRTUAL FROM BASE
	
	void config();		   //show the controller configuration
	
	//get and set target heading
	void set_target_heading(int new_target) {
		target = new_target;
		upper_lim = wrap(new_target, tolerance(), Wrap::up);
		lower_lim = wrap(new_target, tolerance(), Wrap::down);
	}
	int target_heading() {return target;}
	
	//get and set tolerance
	void set_tolerance(int new_tolerance) {tol = new_tolerance;}
	int tolerance() {return tol;}

};

#endif
