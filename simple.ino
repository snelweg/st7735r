int pin_clk = 9;
int pin_data = 8;
int pin_cmd = 7;
int pin_cs = 6;

int led = 13;

void setup() {
  // start
  pinMode(led, OUTPUT);
  
  // setup
  pinMode(pin_clk, OUTPUT);
  pinMode(pin_data, OUTPUT);
  pinMode(pin_cmd, OUTPUT);
  pinMode(pin_cs, OUTPUT);
  
  set(pin_cs, HIGH);
  set(pin_cmd, HIGH);
  
  // button trigger, see explanation in loop()
  attachInterrupt(0, buttonfunc, RISING);
}

volatile boolean doit = false;
void buttonfunc() {
  doit = true;
}

int lasttime = 0;
  
void loop() {
  // blabla so only button press executes this thing
  // with arduinos, the interrupt-enabled pin is said to be 2
  // have it pull-down to gnd with big resistor and button/direct wire it
  // to vcc when you wish to trigger this functionality
  if (millis() - lasttime < 1000) {
    doit = false;
    return;
  }
  if (doit == false) return;
  lasttime = millis();
  doit = false;
  
  // fancy // hyoman operator sees something happened 
  digitalWrite(led, HIGH);
  
  // set slave to listen
  set(pin_cs, LOW);
  
  // send cmd
  set(pin_cmd, LOW);
  out(0x0c, true);
  set(pin_cmd, HIGH);
  
  // read
  byte r1 = in();
  byte r2 = in();

  // done // slave apparently(?) tristates it's data line after
  // cs deactivates. and if we have tristated pin_data,
  // the line is all over the place. in this case no-one cares function-wise,
  // but it's nicer looking when debugging with logic analyzer
  pinMode(pin_data, OUTPUT);
  
  // set slave to not listen
  set(pin_cs, HIGH);
  
  // end fancy
  digitalWrite(led, LOW);
}

void set(int pin, int val) {
  digitalWrite(pin, val);
  delay(1);
}

void out(byte data, boolean expectread) {
  pinMode(pin_data, OUTPUT);
  
  for (int i=7; i>=0; i--) {
    boolean updown = ((data >> i) & 1) == 1;
    
    // bit to data line
    digitalWrite(pin_data, updown == 1 ? HIGH : LOW);
    delay(1);
    
    // tick
    digitalWrite(pin_clk, HIGH);
    delay(1);
    
    // as per datasheet, tristate before last falling clock edge
    if (i == 0 && expectread) {
      digitalWrite(pin_data, LOW);
      delay(1);
      pinMode(pin_data, INPUT);
      delay(1);
    }
    // tock
    digitalWrite(pin_clk, LOW);
    delay(1);
  }
  // return data line to LOW
  //dont care of data line?
  //if (!expectread) {
  //  digitalWrite(pin_data, LOW);
  //}
  delay(3);
}

byte in() {
  byte data;
  
  for (int i=0; i<8; i++) {
    // tick
    digitalWrite(pin_clk, HIGH);
    delay(1);
    
    // bit from data line
    data |= (digitalRead(pin_data) == HIGH ? 1 : 0) << i;
    delay(1);
    
    // tock
    digitalWrite(pin_clk, LOW);
    delay(1);
  }
  
  delay(3);
  return data;
}
