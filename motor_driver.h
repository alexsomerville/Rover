
namespace Rover1
{
    class MotorDriver
    {
    public:
      
        virtual void setSpeed(int speed) = 0;
        
        virtual int getSpeed() const = 0;            
    };
};
