//ARAM_FS_Stepper Libary      -  Jack Serlin                rev: 4/29/2026               A.R.A.M. - American Robotics Assisted Manufacturing


#if defined(ESP32)
  #include <soc/gpio_reg.h>
  #include <soc/io_mux_reg.h>
#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
  #define IS_ARDUINO_ARCH
#else
  #define IS_GENERIC_ARCH
#endif

#define STEPPER ARAM_FS_STEPPER

#define SET_RAW_PIN_OUT_HIGH(reg,a) reg |= (1<<a)
#define SET_RAW_PIN_OUT_LOW(reg,a) reg &= (!(1<<a))



//The following is used to cycle through state of pins when moving the stepper
int ARAMstepperStateA [2] = {0,0};
int ARAMstepperStateB [2] = {1,0};
int ARAMstepperStateC [2] = {1,1};
int ARAMstepperStateD [2] = {0,1};
int * ARAMstepperStatesList [4] = {ARAMstepperStateA,ARAMstepperStateB,ARAMstepperStateC,ARAMstepperStateD};

//ARAM stepper object, represents a stepper
class STEPPER{

    private:

      #if defined(ESP32)
      void fastWrite(uint8_t pin, uint8_t state){
        if((pin < 0)||pin>(48))return;
        if(pin <= 31){
          if(state==HIGH){
            *(volatile uint32_t*)(GPIO_OUT_W1TS_REG) |= 1<<pin;
          }else{
            *(volatile uint32_t*)(GPIO_OUT_W1TC_REG) |= 1<<pin;
          }
          return;
        }

        #if defined(GPIO_OUT1_W1TS_REG)
        if(state==HIGH){
          *(volatile uint32_t*)(GPIO_OUT1_W1TS_REG) |= 1<<(pin - 32);
        }else{
          *(volatile uint32_t*)(GPIO_OUT1_W1TC_REG) |= 1<<(pin - 32);
        }
        return;
        #endif
      }
      #else
      void fastWrite(uint8_t pin, uint8_t state){
        digitalWrite(pin,state);
      }
      #endif

    public:
    STEPPER(){

    }

    STEPPER(STEPPER &other){
        this->pinA = other.pinA;
        this->pinB = other.pinB;
        this->curState = other.curState;
    }

    STEPPER(int pinA, int pinB){
        
        pinMode(pinA,OUTPUT);
        fastWrite(pinA,HIGH);
        pinMode(pinB,OUTPUT);
        fastWrite(pinB,HIGH);

        //store pin data into struct
        this->pinA = pinA;
        this->pinB = pinB;
        this->curState = 0;
    }
    void moveStepperCW();
    void moveStepperCCW();



    private:
    int pinA;
    int pinB;
    int curState;

    void _updateStepperPinOutputs();

};



//updates pin outputs for a stepper motor to reflect the 
void STEPPER::_updateStepperPinOutputs(){
  //grab correct pin states
  int pinAState = ARAMstepperStatesList[this->curState][0];
  int pinBState = ARAMstepperStatesList[this->curState][1];

  //update pin states
  if(pinAState)fastWrite(this->pinA,HIGH);
  else fastWrite(this->pinA,LOW);

  if(pinBState)fastWrite(this->pinB,HIGH);
  else fastWrite(this->pinB,LOW);
}

//Moves Stepper One Step In The Clockwise Direction
//Argument is index of stepper motor, returned by "initARAMStepperMotor"
void STEPPER::moveStepperCW(){
  //cycle to next state
  this->curState = (this->curState+1)%4;
  //update pin outputs
  _updateStepperPinOutputs();
}

//Moves Stepper One Step In The Counter Clockwise Direction
//Argument is index of stepper motor, returned by "initARAMStepperMotor"
void STEPPER::moveStepperCCW(){
  //cycle to next pin state
  this->curState = (this->curState+3)%4;
  //update pin outputs
  _updateStepperPinOutputs();
}
