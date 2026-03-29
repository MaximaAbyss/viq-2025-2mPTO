#include "vex.h"

using namespace vex;

vex::brain       Brain;

//      DRIVETRAIN  
vex::motor dt_tl(PORT11, 1.0,true);
vex::motor dt_tr(PORT5, 1.0,false);
motor dt_bl(PORT1, 1.0,false);
motor dt_br(PORT7, 1.0,true);

motor_group dt_l(dt_tl, dt_bl);
motor_group dt_r(dt_tr, dt_br);


//      PTO
motor pto_l(PORT10, 1.0, true);
motor pto_r(PORT4, 1.0, false);

//      PNEUMATICS
pneumatic l_claw_finger(PORT12);
pneumatic pas_r_claw(PORT6);
pneumatic catch_ninetyone(PORT2);

//      OTHER
vex::controller controller1;

//      SETUP
void setup(){
    dt_l.setStopping(coast);
    dt_r.setStopping(coast);

    pto_l.resetPosition();
    pto_l.setStopping(hold);
    pto_l.setStopping(hold);

    l_claw_finger.extend(cylinder1);
    l_claw_finger.retract(cylinder2);
    pas_r_claw.extend(cylinder1);
    pas_r_claw.retract(cylinder2);
    catch_ninetyone.retract(cylinder2);
}


//      VARIABLES
bool is_dt_stopped;
bool is_l_claw_grabbing = false;
bool is_r_claw_grabbing = true;
bool is_finger_down = false;
bool is_ninetyone_down = true;
bool is_catch_extended = true;
bool is_pas_tech_on = false;
int six_bar_stage = 0;


//      FUNCTIONS
void manual_drive(int x_pos, int y_pos){
    int left_dt_speed = y_pos + x_pos;
    int right_dt_speed = y_pos - x_pos;

    if(abs(left_dt_speed) >=5 || abs(right_dt_speed) >=5){
        dt_tl.spin(forward);
        dt_tr.spin(forward);
        dt_bl.spin(forward);
        dt_br.spin(forward);

        dt_tl.setVelocity(left_dt_speed,pct);
        dt_bl.setVelocity(right_dt_speed,pct);
        dt_tr.setVelocity(right_dt_speed,pct);
        dt_br.setVelocity(left_dt_speed,pct);
        is_dt_stopped = false;
    }
    else if(is_dt_stopped == false){
        dt_l.stop();
        dt_r.stop();
        is_dt_stopped = true;
    }
    else{
        return;
    }
        
}

void catch_allows_lift(){
    if(is_catch_extended){
        catch_ninetyone.retract(cylinder1);
        is_catch_extended =! is_catch_extended;
    }
}

void catch_allows_arms(){
    if(!is_catch_extended){
        catch_ninetyone.extend(cylinder1);
        is_catch_extended =! is_catch_extended;
    }
}

void move_6_bar(int controller_pos){
    pto_l.spin(forward);
    pto_r.spin(reverse);
    pto_r.setVelocity(controller_pos,pct);
    pto_l.setVelocity((-1) * controller_pos,pct);

}

void manual_arm_forward(){
    catch_allows_arms();
    pto_l.setVelocity(100,pct);
    pto_r.setVelocity(100,pct);

    pto_l.spin(reverse);
    pto_r.spin(reverse);
}

void manual_arm_reverse(){
    catch_allows_arms();
    pto_l.setVelocity(100,pct);
    pto_r.setVelocity(100,pct); 

    pto_l.spin(forward);
    pto_r.spin(forward);
  
}

void manual_arm_stop(){
    pto_l.setVelocity(0,pct);
    pto_r.setVelocity(0,pct); 

    pto_l.stop();
    pto_r.stop();
}

void run_six_bar(){

    if(abs(controller1.AxisD.position()) > 5){
        catch_allows_lift();
        move_6_bar(controller1.AxisD.position());
        
    }
    else{
        catch_allows_arms();
        pto_l.stop();
        pto_r.stop();
        
    }
}


void l_claw_move(){
    is_l_claw_grabbing ? l_claw_finger.extend(cylinder1) : l_claw_finger.retract(cylinder1);
    is_l_claw_grabbing =! is_l_claw_grabbing;
}

void r_claw_move(){
    is_r_claw_grabbing ? pas_r_claw.extend(cylinder2) : pas_r_claw.retract(cylinder2);
    is_r_claw_grabbing =! is_r_claw_grabbing;
}

void finger_move(){
    is_finger_down ? l_claw_finger.retract(cylinder2) : l_claw_finger.extend(cylinder2);
    is_finger_down = !is_finger_down;
}

void ninetyone_flip(){
    is_ninetyone_down ? catch_ninetyone.extend(cylinder2) : catch_ninetyone.retract(cylinder2);
    is_ninetyone_down =! is_ninetyone_down;
}

void load_beam(){
    catch_allows_arms();
    pto_l.setVelocity(100,pct);
    pto_r.setVelocity(100,pct);
    pto_l.spinFor(-1.55,turns,false);
    pto_r.spinFor(-1.55,turns,false);
    wait(1.7,sec);
    while(!is_finger_down){
        wait(0.05,sec);
    }
    l_claw_finger.extend(cylinder1);
    is_l_claw_grabbing = false;
    pas_r_claw.retract(cylinder2);
    is_r_claw_grabbing = true;

    wait(0.5, sec);
    pto_l.spinFor(1.55,turns,false);
    pto_r.spinFor(1.55,turns,false);

}

   

void corner_goal(){
    is_pas_tech_on ? pas_r_claw.extend(cylinder1) : pas_r_claw.retract(cylinder1);
    is_pas_tech_on = ! is_pas_tech_on;
}


void setup_callbacks(){
    controller1.ButtonFDown.pressed(l_claw_move);
    controller1.ButtonFUp.pressed(r_claw_move);
    controller1.ButtonEDown.pressed(ninetyone_flip);
    controller1.ButtonR3.pressed(finger_move);
    controller1.ButtonEUp.pressed(corner_goal);

    controller1.ButtonLUp.pressed(load_beam);

    controller1.ButtonRUp.pressed(manual_arm_forward);
    controller1.ButtonRDown.pressed(manual_arm_reverse);
    controller1.ButtonRUp.released(manual_arm_stop);
    controller1.ButtonRDown.released(manual_arm_stop);

    controller1.AxisD.changed(run_six_bar); 
}


int main() {
	
    setup();
    setup_callbacks();
   
    while(1) {
        manual_drive(controller1.AxisB.position(), controller1.AxisA.position());
        this_thread::sleep_for(10);
    }
}
