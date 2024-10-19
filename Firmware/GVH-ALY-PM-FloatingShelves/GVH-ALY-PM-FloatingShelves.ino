#define DEBUG 1

#ifdef DEBUG
# define DEBUG_PRINTLN(x) Serial.println(x);
# define DEBUG_PRINT(x) Serial.print(x);
#else
# define DEBUG_PRINTLN(x)
# define DEBUG_PRINT(x)
#endif

void setup() {
  Serial.begin(9600); delay(10);
  

}

void loop() {
  

}
