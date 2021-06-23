#include"mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"
#include "stdlib.h"
#include "string.h"
#include <math.h>
PwmOut pin5(D5), pin6(D6);
Ticker servo_ticker;
BufferedSerial pc(USBTX,USBRX); //tx,rx
BufferedSerial uart(A1,A0); //tx,rx
DigitalInOut pin10(D10);
BufferedSerial xbee(D1, D0);

BBCar car(pin5, pin6, servo_ticker);

Thread t1;
EventQueue queue(64 * EVENTS_EVENT_SIZE);

char recvall[50], recvall2[50], tmp[50];
int now;
int re = 1;

void Follow();

int main(){
   xbee.set_baud(9600);
   char recv[1];
   uart.set_baud(9600);
   t1.start(callback(&queue, &EventQueue::dispatch_forever));
   queue.call(Follow);
   int a = 0;
   char recv[1];
   while(1){
      if(uart.readable()){
            uart.read(recv, sizeof(recv));
            if(recv[0] == '(') now = 0;
            else if(recv[0] == ')') {
                tmp[now] = recv[0];
                if(re) strcpy(recvall, tmp);
                strcpy(tmp, "");
            }
            else if (recv[0] != ',') {
                tmp[now++] = recv[0];
            }
      } 
   }
}

void Follow(){
    char nx1[4], nx2[4], ny1[4], ny2[4];
    int x1, x2, y1, y2;
    int tz;
    int i = 0;
    int j = 0;
    int count = 0;
    int dx, dy;
    int len;
    float r;
    char buff[25];
    int task = 0;
   
    sprintf(buff, "start 1\r\n");
    xbee.write(buff, 9);
    
    while(1){
        re = 0;
        i = 0;
        len = strlen(recvall);
       
        tz = 0;
        if (recvall[0] == 'a'){
            i = 1;
            while(recvall[i] != ')') {
                nx1[i-1] = recvall[i];
                i++;
            }
            tz = atoi(nx1);
        }
        else {
            i = 0;
            count = 0;
            while(recvall[count] != ' '){
                nx1[i] = recvall[count];
                i++;
                count++;
            }
            i = 0;
            count++;
            while(recvall[count] != ' '){
                ny1[i] = recvall[count];
                i++;
                count++;
            }
            i = 0;
            count++;
            while(recvall[count] != ' '){
                nx2[i] = recvall[count];
                i++;
                count++;
            }
            i = 0;
            count++;
            while(recvall[count] != ')'){
                ny2[i] = recvall[count];
                i++;
                count++;
            }
            i = 0;
        
            x1 = atoi(nx1);
            y1 = atoi(ny1);
            x2 = atoi(nx2);
            y2 = atoi(ny2);
        }
        
        for (i = 0; i < 50; i++) {
            recvall[i] = '\0';
        
        }
        for (i = 0; i < 4; i++) {
            nx1[i] = '\0';
            ny1[i] = '\0';
            nx2[i] = '\0';
            ny2[i] = '\0';
        }
        if (y1 > y2) {
            int temp;
            temp = x1;
            x1 = x2;
            x2 = temp;
            temp = y1;
            y1 = y2;
            y2 = temp;
        }
        re = 1;
        dx = x1 - x2;
        dy = y1 - y2;
        if (abs(dx) + abs(dy) > 0) {
            if (x1 < 65 + task * 5 && task < 2) {
                car.turn(25, 1);
                printf("Right\n");
                ThisThread::sleep_for(50ms);
                car.stop();
                ThisThread::sleep_for(10ms);
            }
            else if (x1 > 95 - task * 5 && task < 2) {
                car.turn(-25, 1);
                printf("LEFT\n");
                ThisThread::sleep_for(50ms);
                car.stop();
                ThisThread::sleep_for(10ms);
            }
            else {
                car.goStraight(35);
            }

            if (tz >= -4 && tz != 0) {
                if (task == 0) {
                    sprintf(buff, "start2\r\n");
                    xbee.write(buff, 7);

                    car.goStraight(100);
                    ThisThread::sleep_for(1500ms);
                    car.stop();
                    ThisThread::sleep_for(1000ms);
                    car.turn(-100, 1);
                    ThisThread::sleep_for(630ms);
                    car.goStraight(100);
                    ThisThread::sleep_for(2500ms);
                    car.stop();
                    ThisThread::sleep_for(1000ms);
                    car.turn(100, 1);
                    ThisThread::sleep_for(680ms);
                    car.goStraight(100);
                    ThisThread::sleep_for(3500ms);
                    car.stop();
                    ThisThread::sleep_for(1000ms);
                    car.turn(100, 1);
                    ThisThread::sleep_for(1000ms);
                    car.goStraight(1000);
                    ThisThread::sleep_for(1300ms);
                    car.stop();
                    ThisThread::sleep_for(1000ms);
                    car.turn(-100, 1);
                    ThisThread::sleep_for(450ms);
                    task = 1;
                    sprintf(buff, "end 2\r\n");
                    xbee.write(buff, 7);

                } else if (task == 1){
                    car.stop();
                    sprintf(buff, "end all\r\n");
                    xbee.write(buff, 9);
                    task++;
                }
            }
            
        }
        else{
            car.stop();
        }
        ThisThread::sleep_for(50ms);
    }
}
