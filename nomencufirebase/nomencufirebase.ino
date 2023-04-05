#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <Firebase.h>
//#include <Ubidots.h>
#include <SoftwareSerial.h>
//Caro//prueba-4793c-default-rtdb.firebaseio.com
//carokey//AIzaSyCdpGJ2DHDOMyTialeOygVWfbI3znAObtc
//AIzaSyDS4rKS-PVnYoLZDeUbLIHqLBqViNbkQyQ
//login-plantlink-default-rtdb.firebaseio.com
//KS8VAlUmis3BUEmonyqpEKyc42k6afAOTc8PjZhE
//conexionnodemcu-default-rtdb.firebaseio.com
//kLOeiz5fSYUocvPYIEkSKpGR5rpUNEDnJdwsKTMN
//#define UBIDOTS_TOKEN "BBFF-VEG3ytXsabVnpOP3XbNC9kQ5FrI6Ig"
//#define DEVICE_LABEL "ESP8266_PlantLink"
#define GAS_LPG 0
#define GAS_CO  1
#define GAS_SMOKE 2

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

#define FIREBASE_HOST "dbsensors-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "AIzaSyCG-8m86VO_aB9kJPkI4xIaa8SMGCYHTCg"
//#define WiFi_SSID "INFINITUM176C";
//#define WiFi_PASS "Sq8Ay7Us1u";
#define LAB_HUMEDAD "Humedad"
#define LAB_TEMP "Temp"
//#define LAB_HIGO "Higometria"
#define LAB_LUZ "Luz"
#define PIN_DHT D4
String ruta = "/Centinel01";


const char* WIFI_SSID = "INFINITUM176C";
const char* WIFI_PASS = "Sq8Ay7Us1u";

const long utcOffsetInSeconds = -21600;
//Ubidots ubidots(UBIDOTS_TOKEN, UBI_HTTP);
DHT dht(PIN_DHT, DHT11);

FirebaseData firebaseData;

float Temp;
int Humedad;
//int Moisture_signal = A0;
int Light_signal = A0;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

String diasSemana[7]={"Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado"};

void setup() {
  Serial.begin(115200);
  dht.begin();
  timeClient.begin(); 

  Serial.println("Calibrating...");
  Ro = MQCalibration(GasPin);
  Serial.println("Hecho!");
  Serial.print("Ro= ");
  Serial.print(Ro);
  Serial.println("kohm/n");
  delay(2000);   
   
  //Temp=0;
  Humedad=0;
  //Higometria=60;
  //Luz=0;  
  //CO2= 20; 
  //ubidots.wifiConnect(WIFI_SSID, WIFI_PASS);   
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Conectando a ....");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);    
  }  
  Serial.println();
  Serial.println("Conectado con la IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true); 
}
  

void loop() {
  //humendad = digitalRead(D2);
  //Serial.println(var_poten);
 /*int Moisture = analogRead(Moisture_signal);
  Higometria = Moisture;*/
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

  timeClient.update();
  // int Light = analogRead(Light_signal);
  // Light = ((Light - 1024)*100)/-1024;
  // Luz = Light;  
   
  Temp = dht.readTemperature();
  Humedad = dht.readHumidity();
  Serial.println("Temperatura=" + String(Temp) + ", Humedad=" + String(Humedad));
  //Serial.println(timeClient.getFormattedTime());
  //String Fecha = timeClient.getFormattedDate();
  String Tiempo = timeClient.getFormattedTime();
  Serial.println(Tiempo);
  //Serial.println(Fecha);
  //ubidots.add(LAB_TEMP, Temp);
  //ubidots.add(LAB_HUMEDAD, Humedad);
  //ubidots.add(LAB_HIGO, Higometria);
  //ubidots.add(LAB_LUZ, Luz);

  //bool bufferSent = false;
  //bufferSent = ubidots.send(DEVICE_LABEL);  

  //if (bufferSent) {
  Serial.println("Valores enviados al dispositivo");
  //}
  Firebase.setInt(firebaseData, ruta + "/Sensores/HumedadV", Humedad);///Valor/valorH
  Firebase.setString(firebaseData, ruta + "/Sensores/HumedadT", Tiempo);///Tiempo/tiempoH  
  Firebase.setFloat(firebaseData, ruta + "/Sensores/TemperaturaV", Temp);///valorT
  Firebase.setString(firebaseData, ruta + "/Sensores/TemperaturaT", Tiempo); ////tiempo 
  // Firebase.setInt(firebaseData, ruta + "/Sensores/IntensidadLuz/Valor", Luz);
  // Firebase.setString(firebaseData, ruta + "/Sensores/IntensidadLuz/Tiempo", Tiempo);
  Firebase.setInt(firebaseData, ruta + "/Sensores/GasV", iPPM_LPG);///Valor/valorG
  Firebase.setString(firebaseData, ruta + "/Sensores/GasT", Tiempo);///Tiempo/tiempoG 
  Firebase.setInt(firebaseData, ruta + "/Sensores/CO2V", iPPM_CO);///Valor/valorC
  Firebase.setString(firebaseData, ruta + "/Sensores/CO2T", Tiempo);///Tiempo/tiempoC 
  Firebase.setInt(firebaseData, ruta + "/Sensores/SmokeV", iPPM_Smoke);///Valor/valorS
  Firebase.setString(firebaseData, ruta + "/Sensores/SmokeT", Tiempo); ///Tiempo/tiempoS
         
  delay(2000);
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
