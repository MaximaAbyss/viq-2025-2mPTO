// Written by Mahin Sujau, the v1 version of the 2m PTO series
#include "vex.h"
#include <string>
#include <vector>
#include <cmath>
#include <sstream>

using namespace vex;

vex::brain       Brain;

//      DRIVETRAIN  
motor dt_tl(PORT7, gearSetting::ratio2_1,false);
motor dt_tr(PORT1, gearSetting::ratio2_1,true);
motor dt_bl(PORT8, gearSetting::ratio2_1,false);
motor dt_br(PORT2, gearSetting::ratio2_1,true);

motor_group dt_l( dt_tl, dt_bl);
motor_group dt_r( dt_tr, dt_br);


//      PTO
motor pto_l(PORT9, gearSetting::ratio1_1, false);
motor pto_r(PORT3, gearSetting::ratio1_1, true);

//      PNEUMATICS
pneumatic claws_r_l(PORT5);
pneumatic aligner_finger(PORT4);
pneumatic six_bar_catch(PORT10);

//      OTHER
controller controller1();

//      SETUP
void setup(){
    dt_l.setStopping(brake);
    dt_r.setStopping(brake)
;
    pto_l.setStopping(hold);
    pto_l.setStopping(hold);
}


//      VARIABLES
bool is_dt_stopped;


//      FUNCTIONS
void manual_drive(int x_pos, int y_pos){
    int left_dt_speed = y_pos + x_pos;
    int right_dt_speed = y_pos - x_pos;

    if(abs(left_dt_speed) >=2 || abs(right_dt_speed) >=2){
        dt_l.spin(forward);
        dt_r.spin(forward);

        dt_l.setVelocity(left_dt_speed,pct);
        dt_r.setVelocity(right_dt_speed,pct);
        is_dt_stopped == false;
    }
    else if(is_dt_stopped == false){
        dt_l.stop();
        dt_r.stop();
        is_dt_stopped == true;
    }
    else{
        return;
    }
        
}

void move_6_bar(int controller_pos){
    pto_l.spin(forward);
    pto_r.spin(reverse);
    pto_l.setVelocity(controller_pos,pct);
    pto_r.setVelocity(controller_pos),pct;

}

void manual_arm(bool up_is_true){
    pto_l.setVelocity(80,pct);
    pto_r.setVelocity(80,pct);

    if (up_is_true){
        pto_l.spin(forward);
        pto_r.spin(forward);
    }
    else{
        pto_l.spin(reverse);
        pto_r.spin(reverse);
    }

}

void run_pto(){

    if(controller1.R1.pressing()){
        manual_arm(true);
    }
    else if(controller1.R2.pressing()){
        manual_arm(false);
    }
    else if(controller1.AxisD.changed()){
        move_6_bar(controller1.AxisD.position())
    }
    else{
        pto_l.stop();
        pto_r.stop();
    }

}

void finger_grab(){
    // six_bar.index() ? 
}



// define your global instances of motors and other devices here


int main() {
	
    Brain.Screen.printAt( 2, 30, "Hello IQ2" );
   
    while(1) {
        
        run_pto();
        this_thread::sleep_for(10);
    }
}
