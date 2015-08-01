#define LOGGING

#define ENABLE_ADAFRUIT_MOTOR_DRIVER
#define ENABLE_NEWPING_DISTANCE_SENSOR_DRIVER


#define RUN_TIME 30                    
#define TOO_CLOSE 10                  
#define MAX_DISTANCE (TOO_CLOSE * 20)   
#define RANDOM_ANALOG_PIN 5            

#ifdef ENABLE_ADAFRUIT_MOTOR_DRIVER
#include <AFMotor.h>
#include "adafruit_motor_driver.h"
#define LEFT_MOTOR_INIT 1
#define RIGHT_MOTOR_INIT 3
#endif

#ifdef ENABLE_NEWPING_DISTANCE_SENSOR_DRIVER
#include <NewPing.h>
#include "newping_distance_sensor.h"
#define DISTANCE_SENSOR_INIT 7,7,MAX_DISTANCE
#endif

#include "logging.h"
#include "moving_average.h"

namespace Rover1
{
    class Robot
    {
    public:
    
        Robot()
            : leftMotor(LEFT_MOTOR_INIT), rightMotor(RIGHT_MOTOR_INIT),
              distanceSensor(DISTANCE_SENSOR_INIT),
              distanceAverage(TOO_CLOSE * 12)
        {
            initialize();
        }
        
        
        void initialize()
        {
            randomSeed(analogRead(RANDOM_ANALOG_PIN));
            endTime = millis() + RUN_TIME * 1000;
            move();
        }
        
        void run()
        {
            if (stopped())
                return;

            unsigned long currentTime = millis();
            int distance = distanceAverage.add(distanceSensor.getDistance());
            log("state: %d, currentTime: %lu, distance: %u\n", state, currentTime, distance);
            
            if (doneRunning(currentTime))
                stop();
            else if (moving()) {
                if (obstacleAhead(distance))
                    turn(currentTime);
            }
            else if (turning()) {
                if (doneTurning(currentTime, distance))
                    move();
            }
        }

    protected:
        void move()
        {
            leftMotor.setSpeed(255);
            rightMotor.setSpeed(255);
            state = stateMoving;
        }
        
        void stop()
        {
            leftMotor.setSpeed(0);
            rightMotor.setSpeed(0);
            state = stateStopped;
        }
        
        bool doneRunning(unsigned long currentTime)
        {
            return (currentTime >= endTime);
        }
        
        bool obstacleAhead(unsigned int distance)
        {
            return (distance <= TOO_CLOSE);
        }
        
        bool turn(unsigned long currentTime)
        {
            if (random(2) == 0) {
                leftMotor.setSpeed(-255);
                rightMotor.setSpeed(255);
            }
            else {
                leftMotor.setSpeed(255);
                rightMotor.setSpeed(-255);
            }
            state = stateTurning;
            endStateTime = currentTime + random(500, 1000);
        }
        
        bool doneTurning(unsigned long currentTime, unsigned int distance)
        {
            if (currentTime >= endStateTime)
                return (distance > TOO_CLOSE);
            return false;
        }
        
        bool moving() { return (state == stateMoving); }
        bool turning() { return (state == stateTurning); }
        bool stopped() { return (state == stateStopped); }

    private:
        Motor leftMotor;
        Motor rightMotor;
        DistanceSensor distanceSensor;
        MovingAverage<unsigned int, 3> distanceAverage;
        enum state_t { stateStopped, stateMoving, stateTurning };
        state_t state;
        unsigned long endStateTime;
        unsigned long endTime;
    };
};

Rover1::Robot robot;

void setup()
{
    Serial.begin(9600);
    robot.initialize();
}

void loop()
{
    robot.run();
}
