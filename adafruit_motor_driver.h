
#include "motor_driver.h"

namespace Rover1
{
    class Motor : public MotorDriver
    {
    public:
       
        Motor(int number)
            : MotorDriver(), motor(number), currentSpeed(0)
        {
        }

        void setSpeed(int speed)
        {
            currentSpeed = speed;
            if (speed >= 0) {
                motor.setSpeed(speed);
                motor.run(FORWARD);
            }
            else {
                motor.setSpeed(-speed);
                motor.run(BACKWARD);
            }
        }
        
        int getSpeed() const
        {
            return currentSpeed;
        }
        
    private:
        AF_DCMotor motor;
        int currentSpeed;
    };
};
