#define GS_NO_ACCEL

typedef long long ll;

#include "GyverPlanner2.h"
#include <ArduinoJson.h>
Stepper<STEPPER2WIRE> left(2, 5);   // X
Stepper<STEPPER2WIRE> right(3, 6);  // Y
GPlanner2<STEPPER2WIRE, 2> base;

int32_t pos[] = {0, 0};

void setup() {
  Serial.begin(115200);
  pinMode(8, OUTPUT);
  digitalWrite(8, 0);

  left.reverse(true);

  base.addStepper(0, left);  // ось 0
  base.addStepper(1, right);  // ось 1

  base.setAcceleration(1000);
  base.setMaxSpeed(2000);
  base.start();
  base.addTarget(pos, 0);
}
const int16_t STEP  = 200 * 16;
float R_WHEEL = 6.4 / 2;            //радиус колеса
float R_BASE = 18.8;            //расстояние между центрами колес (setAngle)
float R_BASE_DUGA = 18.8;       //расстояние между центрами колес (radius_rotation и arc_mov)
float angle_wheels[2];
float base_pos[2];  // X Y
void loop() {
  
  base.tick();
  
  if (Serial.available() > 0) {
    Serial.flush();


    String s = Serial.readString();       // чтение строки и очистка буфера
    Serial.flush();

    
    if (s != ""){
      
      String type;
      int p1 = 0, p2 = 0, p3 = 0;
      String ty;
      int uk = 0;

      
      base.tick();
      
      for (char i : s) {                  // разбиение по переменным 
        base.tick();                      // type - тип команды
        if (i == '|') {                   // p1 - первый аргумент
          if (uk == 0) {                  // p2 - второй аргумент
            type = ty;                    // p3 - третий аргумент
          }
          
          if (uk == 1) {
            p1 = ty.toInt();
          }
          
          if (uk == 2) {
            p2 = ty.toInt();
          }
          
          if (uk == 3) {
            p3 = ty.toInt();
          }
          
          uk += 1;
          ty = "";
        } else {
          ty += i;
        }
      }
      Serial.flush();
      base.tick();

      
      if (type == "mov") {                                      // движение прямо
        mov(p1 / 100.0);
        while (base.tick()) {           
          
        }
        Serial.println("e");
      }
      base.tick();

      
      if (type == "setAngle"){                                  // поворот на месте
        setAngle(p1 / 180.0 * PI);
        while (base.tick()) {
          
        }
        Serial.println("e");
      }
      base.tick();

      
      if (type == "radius_rotation") {                          // поворот одним колесом
        radius_rotation(p1 / 180.0 * PI, p2 == 1);
        while (base.tick()) {
          
        }
        Serial.println("e");
      }
      base.tick();

      
      if (type == "arc_mov") {                                  // движение по дуге
        arc_mov(p1 / 100.0, p2 / 180.0 * PI, p3 == 1);
        while (base.tick()) {
          
        }
        Serial.println("e");
      }

      
      if (type == "changes") {                                  // смена скорости
        base.setMaxSpeed(p1);
        while (base.tick()) {
          
        }
        Serial.println("e");
      }

      
      if (type == "changea") {                                  // смена разгона
        base.setAcceleration(p1);
        while (base.tick()) {
          
        }
        Serial.println("e");
      }


      if (type == "cw") {
        R_WHEEL = p1 / 100.0;
        while (base.tick()) {
          
        }
        Serial.println("e");
      }


      if (type == "cb") {
        R_BASE = p1 / 100.0;
        while (base.tick()) {
          
        }
        Serial.println("e");
      }


      if (type == "cbd") {
        R_BASE_DUGA = p1 / 100.0;
        while (base.tick()) {
          
        }
        Serial.println("e");
      }

      
      type = "";                // обнуление типа
      base.tick();
    }
  }
  base.tick();
}

void mov(float line) { // RELATIVE
  float angle = line / R_WHEEL;
  int32_t stepper_pos = angle / (2 * PI) * STEP;
  pos[0] = stepper_pos;
  pos[1] = stepper_pos;
  base.addTarget(pos, 0, RELATIVE);
  pos[0] = 0;
  pos[1] = 0;
}

void setAngle(float angle) {
  int32_t stepper_pos = (2 * PI * R_BASE * angle/ (2 * PI)) / (2 * PI * R_WHEEL) * STEP / 2;
  pos[0] = stepper_pos;
  pos[1] = -stepper_pos;
  base.addTarget(pos, 0, RELATIVE);
  pos[0] = 0;
  pos[1] = 0;
}

void radius_rotation(float angle, bool ty) {  
  angle *= R_BASE_DUGA / R_WHEEL;
  int32_t stepper_pos = angle / PI * STEP;
  if (ty) {
    pos[1] = stepper_pos;
  } else {
    pos[0] = stepper_pos;
  }
  base.addTarget(pos, 0, RELATIVE);
  pos[0] = 0;
  pos[1] = 0;
}

void arc_mov(float radius, float angle, bool ty) {
  float short_radius = radius;
  float long_radius = radius + 2 * R_BASE_DUGA;
  float short_pos = (2 * PI * short_radius * angle/ (2 * PI)) / (2 * PI * R_WHEEL) * STEP;
  float long_pos = (2 * PI * long_radius * angle/ (2 * PI)) / (2 * PI * R_WHEEL) * STEP;
  if (ty) {
    pos[0] = short_pos;
    pos[1] = long_pos;
  } else {
    pos[1] = short_pos;
    pos[0] = long_pos;
  }
  base.addTarget(pos, 0, RELATIVE);
  pos[1] = 0;
  pos[0] = 0;
}