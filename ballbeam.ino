int ena=11;
int in1=9;
int in2=10;
int giatri;
int bamxung;
const int interrupt0=0;
const int phase_a= 2;
const int phase_b=3;
float angle_ref=0;
const int trigPin = A2;
const int echoPin = A3;
long duration=0;
float distance=0;
float e_x = 0;
float n  = 0;
float xd = 20;
float edot_x = 0;
float pre_e_x = 0;
int x0=18;
long prevT=0;
int pos=0;
int pos_zero=0;
float eprev=0;
float eintegral=0;
float u=0;
int target=0;
float pwr=0;
int dir=0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ena,OUTPUT);
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  attachInterrupt (interrupt0, Read_Encoder, RISING);
  pinMode(phase_a, INPUT_PULLUP);
  pinMode(phase_b, INPUT_PULLUP);
  

 
}
short a = 0;
float ad = 0;

void Read_Encoder(){
  if (digitalRead(3)>0) pos++;
  else pos--;
 
}
float hlt_hinhthang(float x,float L, float C1,float C2, float R)
{
    float y;
    if(x < L)
        y=0;
    else if (x <C1)
        y= (float)((x-L)/(C1-L));
    else if(x <C2)
        y=1;
    else if(x<R)
        y= (float)((R-x)/(R-C2));
    else
        y=0;
    return y;
}
float e_NB, e_NS, e_ZE, e_PS, e_PB, edot_NB, edot_NS, edot_ZE, edot_PS, edot_PB; 
float y_NB, y_NM, y_NS, y_ZE, y_PS, y_PM, y_PB;
float beta1=0, beta2=0, beta3=0, beta4=0, beta5=0, beta6=0, beta7=0;
float beta8=0, beta9=0, beta10=0,beta11=0,beta12=0,beta13=0,beta14=0;
float beta15=0,beta16=0,beta17=0,beta18=0,beta19=0,beta20=0,beta21=0;
float beta22=0,beta23=0,beta24=0,beta25=0;

float fuzzy_from_dis_to_angle(float e, float edot)
{		float y;
	  e_NB=  hlt_hinhthang(e,-1.5,-1,-0.75,-0.2);
    e_NS = hlt_hinhthang(e,-0.75,-0.2,-0.2,0);
    e_ZE = hlt_hinhthang(e,-0.2,0,0,0.2);
    e_PS = hlt_hinhthang(e,0,0.2,0.2,0.75);
    e_PB = hlt_hinhthang(e,0.2,0.75,1.25,1.5);
	
		edot_NB = hlt_hinhthang(edot,-1.5,-1,-0.75,-0.2);
    edot_NS = hlt_hinhthang(edot,-0.75,-0.2,-0.2,0);
    edot_ZE = hlt_hinhthang(edot,-0.2,0,0,0.2);
    edot_PS = hlt_hinhthang(edot,0,0.2,0.2,0.75);
    edot_PB = hlt_hinhthang(edot,0.2,0.75,1.25,1.5);
	
		y_NB = -1;
    y_NM = -0.6;
    y_NS = -0.2;
    y_ZE = 0;
    y_PS = 0.2;
    y_PM = 0.6;
    y_PB =  1;
	
	  beta1 = e_NB*edot_NB;
    beta2 = e_NB*edot_NS;
    beta3 = e_NB*edot_ZE;
    beta4 = e_NB*edot_PS;
    beta5 = e_NB*edot_PB;
    /*e= NS; edot = NB->PB */
    beta6 =e_NS*edot_NB;
    beta7 =e_NS*edot_NS;
    beta8 =e_NS*edot_ZE;
    beta9 =e_NS*edot_PS;
    beta10 =e_NS*edot_PB;
    /*e= ZE; edot = NB->PB */
    beta11 =e_ZE*edot_NB;
    beta12 =e_ZE*edot_NS;
    beta13 =e_ZE*edot_ZE;
    beta14 =e_ZE*edot_PS;
    beta15 =e_ZE*edot_PB;
    /*e= PS; edot = NB->PB */
    beta16 =e_PS*edot_NB;
    beta17 =e_PS*edot_NS;
    beta18 =e_PS*edot_ZE;
    beta19 =e_PS*edot_PS;
    beta20 =e_PS*edot_PB;
    /*e= PB; edot = NB->PB */
    beta21 =e_PB*edot_NB;
    beta22 =e_PB*edot_NS;
    beta23 =e_PB*edot_ZE;
    beta24 =e_PB*edot_PS;
    beta25 =e_PB*edot_PB;
		
		y=( beta1*y_NB+  beta2*y_NB+  beta3*y_NM+  beta4*y_NS+  beta5*y_ZE+
      beta6*y_NB+  beta7*y_NM+ beta8*y_NS+ beta9*y_ZE+ beta10*y_PS+
     beta11*y_NM+ beta12*y_NS+ beta13*y_ZE+ beta14*y_PS+ beta15*y_PM+
     beta16*y_NS+ beta17*y_ZE+ beta18*y_PS+ beta19*y_PM+ beta20*y_PB+
     beta21*y_ZE+ beta22*y_PS+ beta23*y_PM+ beta24*y_PB+ beta25*y_PB
         );
    y=constrain(y, -1, 1);
    return y;
}
void ReadHC_SRF04(){

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  // toc do am thanh trong khong khi la 340 m/s. tuong duong29.412 micro s/ cm
  // do thoi gian tinh từ lúc phat hiện sóng cho tới khi sóng phản xạ lại
  distance = duration * 0.034 / 2;
}
uint16_t ConvertDegToPulse(uint16_t nDeg)
{
	float dPulse = nDeg * 4 * 11 * 21.3 / 360;

	return (uint16_t)dPulse;
}
void setMotor( int dir, int pwmVal)
{
  //dir: direction rotate
  //pwmVal: pwm speed
  // pwm: pwm pin
  analogWrite(ena,pwmVal);
  if(dir==1){
    digitalWrite(in1,HIGH);
    digitalWrite(in2,LOW);
  }
  else if(dir==-1){
    digitalWrite(in1,LOW);
    digitalWrite(in2,HIGH);
  }
  else{
    digitalWrite(in1,LOW);
    digitalWrite(in2,LOW);
  }
}
/*void printf(){
    Serial.print(x0);
  Serial.print("|");
  Serial.print(distance);
  Serial.print("|");
  Serial.print(e_x*40);
  Serial.print("|");
  Serial.print(edot_x);
  Serial.print("|");
  Serial.print(pos);
  Serial.print("|");
  Serial.println(angle_ref);
}
*/
void PID(int target){
  float kp=1;
  float ki=0.1;
  float kd=0.1;
  // time different
  long currT=micros();
  long et=currT-prevT;
  float deltaT=((float)(currT-prevT))/1.0e6;
  prevT=currT;
  
  int e1=pos-target;
  float de1=(e1-eprev)/deltaT;
  eintegral=eintegral+e1*deltaT;

  // control signal
  u=kp*e1+kd*de1+ki*eintegral;
  eprev=e1;
  //if ((e_x<0.1 )&(e_x>-0.1 )) u=0;
}

void loop() {
  // put your main code here, to run repeatedly:
 if(Serial.available())
{
  String data=Serial.readString();
  //x0=Serial.readStringUntil('\n').toInt();
  x0=data.substring(0,2).toInt();
  //pos_zero=data.substring(2,2).toInt();
  //pos_zero=Serial.readString().substring(2,5).toInt();
  
  //x0=Serial.readStringUntil('\n').toInt();
  pos_zero=data.substring(2,5).toInt();
}
  ReadHC_SRF04();
  
  e_x=(distance-x0-3)/32.67*2;
	edot_x=(e_x-pre_e_x)*5;
  angle_ref=(fuzzy_from_dis_to_angle(e_x, edot_x))*150-pos_zero;
  
  
  //target=angle_ref;
  //if ((pos<target+200 )&(pos>target-200 )) target=200;
  //if((distance<20) & (distance>10)) angle_ref=pos_zero;
  target=angle_ref;
 // if ((pos<target+10)&(pos>target-10 )) target=-48;

  PID(target);
  pwr=fabs(u);
  if(pwr>150) pwr=150;
  if (u>5) dir=-1;
  else if (u<-5){
    dir=1;
  }
  else dir=0;
  setMotor(dir,pwr);
  pre_e_x=e_x;
 
  Serial.print(x0);
  Serial.print("|");
  
  
  Serial.print(distance);
  Serial.print("|");
  
  Serial.print(angle_ref);
  Serial.print("|");
  
  
  Serial.print(pos);
  Serial.print("|");

  Serial.print(e_x);
  Serial.print("|");

  Serial.print(edot_x);
  Serial.print("|");
  
  Serial.print(pos_zero);
  
  
  Serial.print("\n");
  
}




