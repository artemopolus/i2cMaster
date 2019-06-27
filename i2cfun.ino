int getAvailableI2Cadress(byte * Addresses)
{
  int count = 0;
  Serial.print("Scanning iteration:\n");
  for (byte address = 15; address < 127; address++)
  {
    Wire.beginTransmission(address);
    byte flag = Wire.endTransmission();
    switch (flag)
    {
      case 0:
        Serial.print("Found I2C device: 0x");
        Serial.println(address, HEX);
        Addresses[count] = address;
        count++;
      break;
      case 4:
        Serial.print("Unknown error at 0x");
        Serial.println(address, HEX);
      break;
    }
  }
  if (count == 0)
    Serial.println("No I2C devices found");
  else
    nDevices = count;
  return count;
}
void scanAddress()
{
  Serial.print("Scanning iteration:\n");
  for (byte address = 15; address < 127; address++)
  {
    Wire.beginTransmission(address);
    byte flag = Wire.endTransmission();
    switch (flag)
    {
      case 0:
        Serial.print("Found I2C device: 0x");
        Serial.println(address, HEX);
        AddressList[nDevices] = address;
        nDevices++;
      break;
      case 4:
        Serial.print("Unknown error at 0x");
        Serial.println(address, HEX);
      break;
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found");
}
void writeData(const byte address, const byte value)
{
  Serial.print("Transmit value: ");
  Wire.beginTransmission(address);
  for(int j = 0; j < 3; j++)
  {
    Wire.write(value);
    Serial.print(value);
    Serial.print(" ");
  }
  Wire.endTransmission(true);
  Serial.println("End transmission");
}
void writeStr(const byte address, byte * values, const int length)
{
  Serial.print("Transmit string: ");
  Wire.beginTransmission(address);
  for(int i = 0; i < length; i++)
  {
    //if(!values[i]) break;
    Wire.write(values[i]);
    Serial.print((char)values[i]);
    Serial.print("[");
    Serial.print(values[i],HEX);
    Serial.print("]");
  }
  Wire.endTransmission(true);
  Serial.println("End transmission");
}
void readData(const byte address,const int nbData)
{
  Wire.requestFrom(address,nbData);
  int nbWord = 0;
  while(Wire.available())
  {
    char c = Wire.read();
    Serial.print("0x");
    Serial.print(c,HEX);
    Serial.print("[");
    Serial.print(c);
    Serial.print("]");
    Serial.print(" ");
    nbWord++;
  }
  Serial.print("\nCharacter count: ");
  Serial.println(nbWord);
}
int readStr(const byte address, const int nbData)
{
  // Wire.requestFrom(address,nbData);
  // int count = Wire.read();
  int count = nbData;
  Serial.print("address=");
  Serial.print(address); Serial.print("[0x"); Serial.print(address, HEX); Serial.println("]\nData:");
  Serial.print("Count=");
  Serial.print(count); Serial.print("[0x"); Serial.print(count, HEX); Serial.println("]\nData:");
  Wire.requestFrom(address,count);
  int counter = 0, i = 0;
  while(!Wire.available())
  {
    if(i < 1000)  i++;
      else break;
  }
  while(Wire.available())
  {
    char word = Wire.read();
    Serial.print(word); Serial.print("[0x"); Serial.print(word, HEX); Serial.print("] ");
    ReceiveBuffer[counter] = (uint8_t)word;
    counter++;
  }
  Serial.print("done! Data count=");
  Serial.println(counter);
  return counter;
}
int readStrData(const byte address, const int nbData)
{
  // Wire.requestFrom(address,nbData);
  // int count = Wire.read();
  int count = nbData;
  Serial.print("address=");
  Serial.print(address); Serial.print("[0x"); Serial.print(address, HEX); Serial.print("]");
  Serial.print("Count=");
  Serial.print(count); Serial.print("[0x"); Serial.print(count, HEX); Serial.println("]");
  int Data2receive = Wire.requestFrom(address,count);
  int counter = 0, i = 0;
  char word;
  while(Wire.available())
  {
    word = Wire.read();
    Serial.print(word); Serial.print("[0x"); Serial.print(word, HEX); Serial.print("] ");
    ReceiveBuffer[counter] = (uint8_t)word;
    counter++;
  }
  Serial.println("done!");
   word = Wire.read();
    Serial.print(word); Serial.print("[0x"); Serial.print(word, HEX); Serial.print("] ");
  
  Serial.print("Data gets=");
  Serial.println(counter);
  Serial.print("Data available=");
  Serial.println(Data2receive);
  return counter;
}
