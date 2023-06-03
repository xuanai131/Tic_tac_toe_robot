
#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>

#define s_pwm 13


int main()
{
    wiringPiSetupPhys();
    softPwmCreate(s_pwm, 0, 50);
    softPwmWrite(s_pwm, 6.0);  //7.8
    //delay(200);
    //softPwmWrite(s_pwm, 0); 
    delay(2000);
    softPwmWrite(s_pwm, 2.5);  //7.8
    delay(200);
    /*while(1)
    {
        softPwmWrite(s_pwm, 7.5);
        delay(1000);
        softPwmWrite(s_pwm, 12.5);
        delay(1000);
        softPwmWrite(s_pwm, 2.5);
        delay(1000);
    }*/

    return 0;
}
