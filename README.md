記得要把bbcar.cpp的turn改成  
void BBCar::turn( double speed, double factor ){  
servo0.set_factor(1);  
servo1.set_factor(1);  
servo0.set_speed(speed);  
servo1.set_speed(speed);  

}
