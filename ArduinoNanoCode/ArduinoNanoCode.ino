#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define GAS_LPG 0
#define GAS_CO  1
#define GAS_SMOKE 2

//DHT temperature & humidity sensors
int PIN_DHT = 4;

//MQ_7 Gas ppm
int GasPin = A0;
int RL_VALUE = 1;
float RO_CLEAN_AIR_FACTOR = 9.86;
int CALIBRATION_SAMPLE_TIMES=50;
int CALIBRATION_SAMPLE_INTERVAL=500;

int READ_SAMPLE_INTERVAL=50;
int READ_SAMPLE_TIMES=5;

float LPGCurve[3] = {2.3,0.21,-0.47};
float COCurve[3] = {2.3,0.72,-0.34};
float SmokeCurve[3] = {2.3,0.53,-0.44};
float Ro = 10;

//Moisture sensor
int MoisturePin = A1;

//Light Sensor
int LightPin = A2;

int minValue = 1023;    // minimum sensor value
int maxValue = 0; // maximum sensor value

DHT dht(PIN_DHT, DHT11);
float Temp;
int Hum;

void setup() {
  Serial.begin(115200);  // initialize serial communication
  dht.begin(); 

  Serial.println("Calibrating...");
  Ro = MQCalibration(GasPin);
  Serial.println("Hecho!");
  Serial.print("Ro= ");
  Serial.print(Ro);
  Serial.println("kohm/n");
  delay(2000);    
  Temp = 0;
  Hum = 0;
}

void loop() {

  long iPPM_LPG = 0;
  long iPPM_CO = 0;
  long iPPM_Smoke = 0;

  iPPM_LPG = MQGetGasPercentage(MQRead(GasPin)/Ro, GAS_LPG);
  iPPM_CO = MQGetGasPercentage(MQRead(GasPin)/Ro, GAS_CO);
  iPPM_Smoke = MQGetGasPercentage(MQRead(GasPin)/Ro, GAS_SMOKE);     

  Serial.println("-------- Concentration Gases --------");

  Serial.print("LPG: ");
  Serial.print(iPPM_LPG);
  Serial.println(" ppm");

  Serial.print("CO: ");
  Serial.print(iPPM_CO);
  Serial.println(" ppm");
  
  Serial.print("Smoke: ");
  Serial.print(iPPM_Smoke);
  Serial.println(" ppm");

  Serial.println("----------------------");  

  int Moisturesensor = analogRead(MoisturePin); // read the sensor value
  int MoisturePercentage = (Moisturesensor - minValue) * 100 / (maxValue - minValue); // convert to percentage
  Serial.print("Moisture value: ");
  Serial.print(Moisturesensor);
  Serial.print(", Percentage: ");
  Serial.print(MoisturePercentage);
  Serial.println(" %");

  Serial.println("----------------------");

  int Lightsensor = analogRead(LightPin); // read the sensor value
  int LightPercentage = ((Lightsensor - 1024)*100)/-1024; // convert to percentage
  Serial.print("Light value: ");
  Serial.print(Lightsensor);
  Serial.print(", Percentage: ");
  Serial.print(LightPercentage);
  Serial.println(" %");

  Serial.println("----------------------");

  Temp = dht.readTemperature();
  Serial.print("Temperature: ");
  Serial.print(Temp);
  Serial.println(" ºC");

  Serial.println("----------------------");

  Hum = dht.readHumidity();
  Serial.print("Humidity: ");
  Serial.print(Hum);
  Serial.println(" %");
  Serial.println("");
  Serial.println("----------------------");
  Serial.println("----------------------");  
  Serial.println("");
    
  delay(3000); // wait for a second before reading the sensor again
}

float MQResistanceCalculation(int raw_adc)
{
return (((float)RL_VALUE*(1023-raw_adc)/raw_adc));
}

float MQCalibration(int mq_pin)
{
  int i;
  float val=0;

  for (i=0;i<CALIBRATION_SAMPLE_TIMES;i++){
    val += MQResistanceCalculation(analogRead(mq_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val/CALIBRATION_SAMPLE_TIMES;
  val = val/RO_CLEAN_AIR_FACTOR;
  return val;    
}

float MQRead(int mq_pin)
{
  int i;
  float rs=0;

  for(i=0;i<READ_SAMPLE_TIMES;i++) {
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);    
  }

  rs = rs/READ_SAMPLE_TIMES;
  return  rs;
}

long MQGetGasPercentage(float rs_ro_ratio, int gas_id)
{
if (gas_id == GAS_LPG){
  return MQGetPercentage(rs_ro_ratio, LPGCurve);  
} else if (gas_id == GAS_CO) {
    return MQGetPercentage(rs_ro_ratio, COCurve);
} else if (gas_id == GAS_SMOKE) {
    return MQGetPercentage(rs_ro_ratio, SmokeCurve);        
}
return 0;
}

long MQGetPercentage(float rs_ro_ratio, float *pcurve)
{
  return  
 (pow(10,(((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
}