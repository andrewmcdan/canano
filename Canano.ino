// this sketch was brutally hacked together by TJ Bruno --- https://medium.com/@tbruno25

#include <mcp_can.h>
#include <SPI.h>

unsigned long int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];

bool displayOnce;
bool displaySend;
bool x;
bool msgInput;
bool idInput;
bool lengthInput;
bool selection;

byte idLength;

char id[10];
char msg[35];
unsigned char byteMsg[9];

int r;
int t;
int y;
int byteLength;
long int byteId;

String command;

#define CAN0_INT 2                              // Set INT to pin 2
MCP_CAN CAN0(10);                               // Set CS to pin 10


void setup()
{
  Serial.begin(115200);
  if (CAN0.begin(MCP_ANY, CAN_125KBPS, MCP_8MHZ) == CAN_OK)
  {
    CAN0.setMode(MCP_NORMAL);
    pinMode(CAN0_INT, INPUT);
  }
}

void loop1()
{
  if (displayOnce == false) {
    menu();
  }
  while (selection == false)
  {
    if (Serial.available())
    {
      command = Serial.readStringUntil('\n');
      if (command.equals("receive"))
      {
        selection = true;
        loop();
      }
      else if (command.equals("send"))
      {
        selection = true;
        for (int x = 0; x < 50; x++) Serial.println();
        send();
      }
    }
  }
}


void send()
{
  sendMenu();
  clear();
  for (int x = 0; x < 1; x++)
  {
  if (byteId > 2048)
  {
    r = 1;
  }
  else
  {
    r = 0;
  }  
    Serial.print("id = "); Serial.print(byteId, HEX);
    Serial.print("     length =  "); Serial.print(byteLength);
    Serial.print("     msg =  "); while (t < byteLength)
    {
      Serial.print(byteMsg[t], HEX); Serial.print("  "); t++;
    }
    Serial.println(); Serial.println();  Serial.println("To send , press [enter].");
    Serial.println(); Serial.println();
  }
  while (msgInput = true)
  {
    if (Serial.available())
    {
      command = Serial.readStringUntil('\n');
      if (command.equals(""))
      {
        byte sndStat = CAN0.sendMsgBuf(byteId, r, byteLength, byteMsg);
        Serial.println("Message Sent!");
      }
      if (command.equals("C") or command.equals("c"))
      {
        clear();
        t = 0;
        byteLength = "";
        displayOnce = false;
        msgInput = false;
        selection = false;
        loop1();
      }
    }
  }
}


void loop()
{
  while (!digitalRead(CAN0_INT))
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);
    Serial.print("FRAME:ID=");
    if ((rxId & 0x80000000) == 0x80000000)
    {
      Serial.print(rxId & 0x1FFFFFFF);
    }
    else
    {
      Serial.print(rxId);
    }
    Serial.print(":LEN=");
    Serial.print(len);
    char msgString[3];
    for (byte i = 0; i < len; i++)
    {
      Serial.print(":");
      snprintf(msgString, 3, "%02X", rxBuf[i]);
      Serial.print(msgString);
    }
    if (Serial.available())
    {
      command = Serial.readStringUntil('\n');
      if (command.equals("C") or command.equals("c"))
      {
        clear();
        selection = false;
        x = false;
        loop1();
      }
    }
    Serial.println();
  }
  if (digitalRead(CAN0_INT))
  {
    if (x == false)
    {
      clear();
      Serial.println("|||||   Nothing to read   |||||   or    |||||   Can bus not detected    |||||");
      for (int x = 0; x < 12 ; x++)Serial.println();
      x = true;
    }
  }
  if (Serial.available())
  {
    command = Serial.readStringUntil('\n');
    if (command.equals("C") or command.equals("c"))
    {
      clear();
      selection = false;
      x = false;
      loop1();
    }
  }
}


void clear()
{
  while (Serial.available())
  {
    char t = Serial.read();
  }
  for (int x = 0; x < 50; x++) Serial.println();      // clear the monitor text
  displayOnce = false;                                //display menu options
}

void menu()
{
  delay(750);
  Serial.println(); Serial.println(); Serial.println();
  Serial.println("Type 'receive' to monitor the bus");
  Serial.println("                or");
  Serial.println("Type 'send' to transmit a message");
  Serial.println(); Serial.println(); Serial.println();
  delay(1000);
  Serial.println("Type 'C' to return back to this menu");
  Serial.println(); Serial.println(); Serial.println(); Serial.println();
  displayOnce = true;
  return;
}


void sendMenu()
{
  Serial.println("What is the can id?");
  Serial.println(); Serial.println(); Serial.println();
  while (idInput == false)
  {
    if (Serial.available())
    {
      Serial.readBytesUntil('\n', id, 10);
      unsigned long k;
      char *endptr;
      k = strtol(id, &endptr, 16);
      byteId = k;
      idInput = true;
    }
  }
  Serial.println("How many bytes is the message?");
  Serial.println(); Serial.println(); Serial.println();
  while (lengthInput == false)
  {
    if (Serial.available())
    {
      byteLength = Serial.parseInt();
      if ((byteLength > 0) && (byteLength < 9))
      {
      lengthInput = true;
      }
    }
  }
  Serial.print("Input the message data"); Serial.print("                  ");
  Serial.print("***NOTE: "); Serial.print("Use the following format:    A1,B2,C3,etc."); Serial.println();
  while (msgInput == false)
  {
    if (Serial.available())
    {
      Serial.readBytesUntil(',', msg, 3);
      int z;
      char *endptr;
      z = strtol(msg, &endptr, 16);
      byteMsg[y - 1] = z;
      y++;
    }
    if (y > byteLength )
    {
      idInput = false;
      lengthInput = false;
      msgInput = true;
      y = 0;
    }
  }
}







/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
