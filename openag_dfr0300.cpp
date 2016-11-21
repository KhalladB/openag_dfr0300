/* Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

 #include "openag_dfr0300.h"
 // #include "openag_ds18b20.h"
    
 /*Ds18b20::Ds18b20(5) : _oneWire(5) {
   _sensors = DallasTemperature(&_oneWire);
   _sensors.setWaitForConversion(false);
 }*/
 
 Dfr0300::Dfr0300(int ec_pin){
   _ec_pin = ec_pin;
   status_level = OK;
   status_msg = "";
 }
 
 //Dfr0300::water_temp(int _w_pin) 
 //}
 //OneWire::_w_oneWire(5);

 void Dfr0300::begin(){
   //Serial.begin(9600);
   //Serial2.println("Hi");
   _time_of_last_query = 0;
   _ec_calibration_offset = 0.15;
 }

/*void Ds18b20::update() {
  if (_waiting_for_conversion) {
    if (_sensors.isConversionComplete()) {
      status_level = OK;
      status_msg = "";
      _waiting_for_conversion = false;
      _water_temperature = _sensors.getTempC(_address);
      _send_water_temperature = true;
    }
    else if (millis() - _time_of_last_query > _min_update_interval) {
      status_level = ERROR;
      status_msg = "Sensor isn't responding to queries";
    }
  }
  if (millis() - _time_of_last_query > _min_update_interval) {
    _sensors.requestTemperatures();
    _waiting_for_conversion = true;
    _time_of_last_query = millis();
  }
}*/
 
 void Dfr0300::update(){
   if (millis() - _time_of_last_query > _min_update_interval){
     //Serial2.println("HiUpdate");
     getData();
     _time_of_last_query = millis();
   }
 }
 
 bool Dfr0300::get_water_electrical_conductivity(std_msgs::Float32 &msg){
   msg.data = _water_electrical_conductivity;
   bool res = _send_water_electrical_conductivity;
   _send_water_electrical_conductivity = false;
   return res;
 }

//.......................................Private.......................................//

 float Dfr0300::getData(void){
   //Serial2.println("HiData");
   int analog_sum = 0;
   const int samples = 40;
   _send_water_electrical_conductivity = true;
   for (int i = 0; i<samples; i++){
     analog_sum += analogRead(_ec_pin);
     delay(2);
   }
   float analog_average = (float) analog_sum / samples;
   float analog_voltage = analog_average*(float)5000/1024;
   float temperature_coefficient = 1.0 + 0.0185*(23.5 - 25.0);
   float voltage_coefficient = analog_voltage / temperature_coefficient;
   if(voltage_coefficient < 0) {
    _water_electrical_conductivity = 0;
    //Serial.println("No solution!");   //25^C 1413us/cm<-->about 216mv  if the voltage(compensate)<150,that is <1ms/cm,out of the range
  }
  else if (voltage_coefficient > 3300) {
    Serial2.println("Out");
    _water_electrical_conductivity = 0;
    //Serial.println("Out of the range!");  //>20ms/cm,out of the range
  }
  else { 
    if(voltage_coefficient <= 448) {
      _water_electrical_conductivity = (6.84*voltage_coefficient-64.32)/1000 + _ec_calibration_offset;
      /*Serial2.print("448: ");
      Serial2.println(_water_electrical_conductivity); 
      return (_water_electrical_conductivity);   //1ms/cm<EC<=3ms/cm*/
    }
    else if (voltage_coefficient <= 1457) {
      _water_electrical_conductivity = (6.98*voltage_coefficient-127)/1000 + _ec_calibration_offset;
      /*Serial2.print("1457: ");
      Serial2.println(_water_electrical_conductivity);
      return (_water_electrical_conductivity);  //3ms/cm<EC<=10ms/cm*/
    }
    else {
      _water_electrical_conductivity = (5.3*voltage_coefficient+2278)/1000 + _ec_calibration_offset;
      /*Serial2.print("else: ");
      Serial2.println(_water_electrical_conductivity); 
      return (_water_electrical_conductivity); //10ms/cm<EC<20ms/cm*/
    }
   }
   return (_water_electrical_conductivity);
 }

 /*float Dfr0300::averageArray(int* arr, int number){
   Serial2.println("HiAverage");
   int i;
   int max,min;
   float avg;
   long amount=0;
   if (number<=0){ //if
     return 0;
   } //if(number<=0)
  
   if (number<5){ // less than 5, calculated directly statistics
     for(i=0;i<number;i++){ //if
       amount+=arr[i];
     } //for (i=0;i<number;i++)
      avg = amount/number;
      return avg;
   } //if (number<5)
  
   else {  //first_else
    
       if(arr[0]<arr[1]){// if(arr[0]<arr[1])
         min = arr[0];
         max = arr[1];
       } // if(arr[0]<arr[1])
    
       else{ // second_else
         min = arr[1];
         max = arr[0];
       } // second_else
    
       for (i=2; i<number; i++){//for (i=2; i<number; i++)
         if (arr[i]<min){ //if (arr[i]<min)
           amount+=min;
           min=arr[i];
         } //if (arr[i]<min)
         else 
         { // third_else
           if (arr[i]>max){ // if (arr[i]>max)
             amount+=max;
             max=arr[i];
           } //if (arr[i]>max)
          
           else
           { //fourth_else
             amount+=arr[i];
           } //fourth_else
         } //third_else
       } //for (i=2; i<number; i++)
       avg = (float) amount/(number-2);
     } // first_else 
  return avg;
} //end of function*/


 
 /*float Ds18b20::getData(void){
  if (_waiting_for_conversion) {
    if (_sensors.isConversionComplete()) {
      status_level = OK;
      status_msg = "";
      _waiting_for_conversion = false;
      _water_temperature = _sensors.getTempC(_address);
      return (_water_temperature);
    }
    else if (millis() - _time_of_last_query > _min_update_interval) {
      status_level = ERROR;
      status_msg = "Sensor isn't responding to queries";
    }
  }
  if (millis() - _time_of_last_query > _w_min_update_interval) {
    _sensors.requestTemperatures();
    _waiting_for_conversion = true;
    _time_of_last_query = millis();
  }
  return (_water_temperature);
 }*/
