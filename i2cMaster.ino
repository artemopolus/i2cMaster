#include <Wire.h>

#define SMPL_SENDER
#define DURATION_0s01  10000 
#define DURATION_1s00  1000000 
#define DURATION_TIMER DURATION_1s00  //0.01 s

int EnableRead = 0;

byte AddressList[115] = {0};
int nDevices = 0;
int powerPin = 11;
int ctrlPin = 10;
char message[10] = {0};

int NbBytesSend = 5;

int IncomingByte = 0;

#define EXREAD  0x01
#define EXWRITE 0x02

uint8_t TestMessage[] = {0,EXWRITE,0,0};

#define RECEIVEBUFFER_CNT 80

uint8_t ReceiveBuffer[RECEIVEBUFFER_CNT] = {0};

int CurrentAddress = 0;

/* для таймера */
hw_timer_t * timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
/* счетчики */
volatile uint32_t isrCounter = 0;
volatile uint32_t lastIsrAt = 0;

void IRAM_ATTR onTimer(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  isrCounter++;
  lastIsrAt = millis();
  portEXIT_CRITICAL_ISR(&timerMux);
  // Give a semaphore that we can check in the loop
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
  // It is safe to use digitalRead/Write here if you want to toggle an output
}

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(115200);
  Serial.print("\nSetup i2c sacnner\n");

#ifdef PIN_CTRL

  pinMode(powerPin, OUTPUT);
  pinMode(ctrlPin, INPUT);
  digitalWrite(powerPin, HIGH);
#endif


  while(!getAvailableI2Cadress(AddressList)){
    delay(1000);
  }

  //init timer
  /* настройка таймера*/
  // Create semaphore to inform us when the timer has fired
  timerSemaphore = xSemaphoreCreateBinary();

  // Use 1st timer of 4 (counted from zero).
  // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more
  // info).
  timer = timerBegin(0, 80, true);

  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer, true);

  // Set alarm to call onTimer function every second (value in microseconds).
  // Repeat the alarm (third parameter)
  timerAlarmWrite(timer, DURATION_TIMER, true);

  // Start an alarm
  timerAlarmEnable(timer);

}

void loop() {
  #ifdef SMPL_SENDER
  if(Serial.available() > 0)
  {
    IncomingByte = Serial.read();
    switch(IncomingByte)
    {
      case 'a':
      byte str[4];
      for(int i = 0; i < 4; i++)
      {
        str[i] = TestMessage[i];
      }
      for(int i = 0; i < nDevices; i++)
      {
        writeStr(AddressList[i],str,4);
      }
      break;
      case 'b':
      for(int i = 0; i < nDevices; i++)
      {
        if(!readStrData(AddressList[i], RECEIVEBUFFER_CNT)) Serial.println("Nothing to read");
      }
      break;
      case 'c':
      EnableRead = 1;
      byte msgcmd[] = {0,EXWRITE,0,1};
      for(int i = 0; i < nDevices; i++)
      {
        writeStr(AddressList[i],msgcmd,4);
      }
      Serial.println("Enable cycling reading");
      break;
    }
  }

  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE){
    if(EnableRead){
      for(int i = 0; i < nDevices; i++)
      {
        if(!readStrData(AddressList[i], RECEIVEBUFFER_CNT)) Serial.println("Nothing to read");
      }
    }else{
      Serial.println("Doing nothing");
    }
  }
  
  #else
    // считываем из порта 0, и отсылаем с порта 1
  int Bytes2Read = Serial.available();
  
  if(Bytes2Read)
  {
    Serial.print("Bytes to read: ");
    Serial.println(Bytes2Read);
    for (int i = 0; i < Bytes2Read; i ++)
    {
      message[i] = Serial.read();
    }
    switch(message[0])
    {
      case 'b':
      if(Bytes2Read > 5)
      {

        byte str[3];
        CurrentAddress = (int)message[1];
        for(int i = 0; i < 3; i++)
        {
          str[i] = message[i + 2];
          writeStr(AddressList[CurrentAddress],str,3);
        }
      }
      break;
      case 'a':
      if(Bytes2Read > 3)
      {
      }
      break;
      default:
      Serial.print(message);
    }   
  }
  if(CurrentAddress){
    int NbBytesReceived_I2C = readStr(AddressList[CurrentAddress],NbBytesSend);
    if(NbBytesReceived_I2C)
    {
      Serial.print("Received data: ");
      for(int i = 0; i < NbBytesReceived_I2C; i++){
        char c = ReceiveBuffer[i];
        Serial.print(c);
        Serial.print("[");
        Serial.print(c,HEX);
        Serial.print("]");
      }
      Serial.print('\n');
    }
  }

  // if (Serial.available()) {
  //   char inByte = Serial.read();
  //   Serial.print(inByte);
  // }


//   // put your main code here, to run repeatedly:
//   Serial.println("Start new iteration");
//   nDevices = 0;
// #ifdef PIN_CTRL
//   if(digitalRead(ctrlPin) == HIGH)
//   {
// #endif
//     Serial.println("Ctrl 1 is on");
//     scanAddress();
//     for (int i = 0; i < nDevices; i++)
//     {
//       writeData(AddressList[i],0x1F);
//       //delay(200);
//       readData(AddressList[i],10);
//       // delay(200);
//       // readData(AddressList[i],5);
//       //readStr(AddressList[i]);
//     }
// #ifdef PIN_CTRL
//   }
// #endif
//   delay(2000);
#endif
}
