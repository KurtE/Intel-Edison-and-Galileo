#define PING_PIN 2
#define DBG_PIN 3
#include <trace.h>
// Quick and dirty ping
void setup() {
  delay(2000);
  Serial.begin(115200);
  delay(500);
  Serial.println("Hello World");
  pinMode(PING_PIN, OUTPUT_FAST);
  pinMode(DBG_PIN, OUTPUT_FAST);
  Serial.println("After PinModes");

}
unsigned long ulPMS, ulPME;

unsigned long DoPing() {
  digitalWrite(DBG_PIN, LOW);  // make sure we are low...
  pinMode(PING_PIN, OUTPUT_FAST);
  digitalWrite(PING_PIN, HIGH);
  delayMicroseconds(3);
  digitalWrite(PING_PIN, LOW);
  //  delayMicroseconds(1);
  ulPMS = micros();
  pinMode(PING_PIN, INPUT_FAST);
  ulPME = micros();

  // Now sure how many yet... 
  unsigned long ulLoopCnt = 1000000;
  while(!digitalRead(PING_PIN) && --ulLoopCnt)
    ;  // maybe yield...

  unsigned long ulStart = micros(); 
  if (!ulLoopCnt)
    return (unsigned long)-1;
  digitalWrite(DBG_PIN, HIGH);  

  ulLoopCnt = 1000000;
  while(digitalRead(PING_PIN) && --ulLoopCnt)
    ;  // maybe yield...

  unsigned long ulEnd = micros(); 
  if (!ulLoopCnt)
    return (unsigned long)-1;
  digitalWrite(DBG_PIN, LOW);  

  return (ulEnd - ulStart);
}  
// Not defined in trace.h but in trace.c
extern "C" void trace_set_level(trace_level_t new_tlevel);

int iTraceLevel = (int)VARIANT_TRACE_LEVEL;
unsigned long ulPMSum = 0;
unsigned long ulPMCnt = 0;

void loop() {
  if (Serial.available()) {
    // Quick and dirty assume one digit value we will use to set Trace level...
    int ch = Serial.read();
    if (ch == '-') {
      iTraceLevel = -1;
      trace_enable(0);  // turn off tracing;
    } 
    else if ((ch >= '0') && (ch <= '2')) {
      trace_enable(1);
      iTraceLevel = ch - '0';
      trace_set_level((trace_level_t)iTraceLevel);
    }
    while (Serial.read() != -1)
      ;
    ulPMSum = 0;  
    ulPMCnt = 0;
  }

  // do the actual ping.
  unsigned long ulPing = DoPing();

  Serial.print("TL= ");
  Serial.print(iTraceLevel, DEC);
  Serial.print(" DTP= ");
  ulPME -= ulPMS;
  ulPMSum += ulPME;
  ulPMCnt++;
  Serial.print(ulPME, DEC);
  Serial.print("(");
  Serial.print(ulPMSum/ulPMCnt, DEC);
  Serial.print(") Ping=");
  Serial.println(ulPing, DEC);
  delay(1000);  
}


