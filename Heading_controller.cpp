#include "Heading_controller.h"

//************************************************************************
//*                     CLASS HELEN_CONTROLLER
//************************************************************************
Heading_controller::Heading_controller(int number_of_sensors, 
	int number_of_plants,
	int _target_heading,
	int _tolerance)
	:Controller(number_of_sensors, number_of_plants), 
	target(_target_heading), tol(_tolerance)
{
	upper_lim = wrap(target, tol, Wrap::up);
	lower_lim = wrap(target, tol, Wrap::down);
}

//*******************************************************************
//*                         REQUIRED VIRTUAL FUNCTIONS
//*******************************************************************
void Heading_controller::run_sensors() 
{
    for(int i = 0; i < sensors.size(); ++i) {
        sensors[i]->run();
        #if DEBUG_RUN == 1
            Serial.print("running sensor: ");
            Serial.println(sensors[i]->name() );
        #endif
    }
}

void Heading_controller::calc_plant_inputs() 
{
    //read the feedback object data into the local copy
    c_msg = c_fb->message();
   #if DEBUG_MESSAGE == 1
		Serial.println(F("From DEBUG_MESSAGE"));
		Serial.println(F("\tthe feedback messages contain: "));
		c_msg.print();
	#endif
	
    active_control_vec = set_control_vec();
	
	#if DEBUG_CONTROL == 1
	    Serial.println(F("From DEBUG_CONTROL"));
	    char buf[50];
	    sprintf(buf, "\tthe active control vec label is %s", active_control_vec.label);
	    Serial.println(buf);
	#endif
		
}

void Heading_controller::update_plant_msgs() 
{
    //update the local copy of the Rover_plant_msg and use it
    //to update the Rover_plant_fb object
    p_msg.l_dir = active_control_vec.l_dir;
    p_msg.l_spd = active_control_vec.l_pwm;
    p_msg.r_dir = active_control_vec.r_dir;
    p_msg.r_spd = active_control_vec.r_pwm;
    
    p_fb->update(p_msg);
}

Control_vec Heading_controller::set_control_vec() 
{	
	Control_vec res;

	res = Control::forward_easy;
	
	int current_Z = c_msg.heading;
	int dZ = delta(current_Z);
	
	//if the heading is within tolerance do nothing
	if (current_Z < upper_lim && current_Z > lower_lim) {
		res = Control::forward_easy;
	}
	
    //if the heading is within ten degrees then ease to the target
	if (dZ >-10 && dZ <(-1*tolerance()) ){
		res = Control::ease_right;
	}
	if (dZ <10 && dZ > tolerance() ){
		res = Control::ease_left;
	}
	
	//if the heading is greater than or equal to ten degrees then swerve
	if (dZ <= -10 ){
		res = Control::swerve_right;
	}
	if (dZ >= 10 ){
		res = Control::swerve_left;
	}
	
	return res;
}

bool Heading_controller::attach(Sensor* s) 
{
    bool attached = false;
    
    char* compass_name = "CP";
    
    //configure sensor handles
    if(!strcmp(s->name(), compass_name) ){
       //cast the Sensor ptr as a Compass*
        Compass* s_pointer = static_cast<Compass*>(s);
       //then get a pointer to the right feedback object
        c_fb = s_pointer->feedback();       
        sensors.push_back(s);
        attached = true;
    }
	
    return attached;
}

bool Heading_controller::attach(Plant* p) 
{
    bool attached = false;

    char* plant_name = "PL";

      //configure Rover_plant handles
      if(!strcmp(p->name(), plant_name) ){ //strcmp returns 0 on equal match
         //cast the Plant* to a Rover_plant* if the name matches
          Rover_plant* p_pointer = static_cast<Rover_plant*>(p);
         //then get a pointer to the right feedback object
          p_fb = p_pointer->feedback(); 
          plants.push_back(p);
          attached = true;
     }
	    
     return attached;
}

void Heading_controller::update_state() 
{
	/*
	    TODO make this a part of the base class so the flow is more obvious
	    from the base class to understand how the system works.
	*/
	run_sensors();

	calc_plant_inputs();

	update_plant_msgs();
}

void Heading_controller::config() 
{
	Serial.println(F("\nHeading controller config: "));
	Serial.print(F("\tSensors attached are: \n"));
	for(int i=0; i<sensors.size(); i++){
		Serial.print(F("\t\t"));
		Serial.println(sensors[i]->name());
	}
	Serial.print(F("\tPlants attached are: \n"));
	for(int i=0; i<plants.size(); i++){
		Serial.print(F("\t\t"));
		Serial.println(plants[i]->name());
	}
	Serial.println(F("*** End Config ***"));
}

//************************************************************************
//*                         HELPER FUNCTIONS
//************************************************************************
int Heaing_controller::wrap(int heading, int tolerance, Wrap::dir wd) 
{
	int res = 0;
	
	if (wd == Wrap::up){
		res = heading + tolerance;
		if (res > 359) {
			res = res - 360;
		}
	} else {
		res = heading - tolerance;
		if (res <0) {
			res = res + 360
		}
	}
	return res;
}

int delta(int current_Z) 
{
	/*
	*  Before making changes to this function, use the delta_test tool
	*  in the /test directory to ensure you get expected results for 
	*  in accordance with the notation that a negative delta indicates
	*  current heading is left of target and a positive delta indicates
	*  current heading is right of target.
	*/
	
	int res = current_Z - target_heading();
	
	if (res < -180){
		res = (360 + res);
	}
	
	if (res > 180){
		res = -(360 - res);
	}
	
	return res;
}





