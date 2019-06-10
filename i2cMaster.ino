#include <Wire.h>

byte AddressList[115] = {0};
int nDevices = 0;
int powerPin = 11;
int ctrlPin = 10;
char message[10] = {0};

int NbBytesSend = 5;

uint8_t ReceiveBuffer[100] = {0};

int CurrentAddress = 0;

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
}

void loop() {
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
}
