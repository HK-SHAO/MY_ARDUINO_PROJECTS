#include <IRremote.h>
#include <U8glib.h>

#define BUZZER 10//喇叭针脚

U8GLIB_ST7920_128X64_4X u8g(13, 12, 11);//配置屏幕针脚

int base = 0, ms = 0, s = 0, mi = 0, hr = 6, week = 0;//进行初始化时间
int nMi = 0, nHr = 8;//闹钟时间
double tem = 0;//温度
char mode = 0;//模式
char mode2 = 0;

int RECV_PIN = 9;
IRrecv irrecv(RECV_PIN);
decode_results results;
 
void setup() {
  pinMode(BUZZER, OUTPUT);
  u8g.setRot180();//旋转屏幕180度
  irrecv.enableIRIn(); //启动红外解码
  base = millis();//初始化基准时间
}

void loop() {
  if (mode == 0) calc();
  else {
    if (hr > 23) hr = 0; else if (hr < 0) hr = 23;
    if (mi > 59) mi = 0; else if (mi < 0) mi = 59;
    if (s > 59)  s = 0; else if (s < 0) s = 59;
    if (week > 6) week = 0; else if (week < 0) week = 6;
    if (nHr > 23) nHr = 0; else if (nHr < 0) nHr = 23;
    if (nMi > 59) nMi = 0; else if (nMi < 0) nMi = 59;
  }

  func();//处理函数
  u8g.firstPage(); do draw(); while (u8g.nextPage());//刷新屏幕

  //是否接收到解码数据,把接收到的数据存储在变量results中
  if (irrecv.decode(&results)) {
    switch (results.value) {
      case 150409965: {//遥控器右按键
          mode2 = 1;
          mode++;
          if (mode > 6) mode = 0;
        }; break;

      case 150419655: {//遥控器左按键
          mode2 = 2;
          mode--;
          if (mode < 0) mode = 6;
        }; break;

      case 150414045: {//遥控器上按键
          mode2 = 3;
          switch (mode) {
            case 0: break; //彩蛋
            case 1: hr++;  break;//设置小时
            case 2: mi++;  break;//设置分钟
            case 3: s++;  break;//设置秒
            case 4: week++;  break;//设置星期
            case 5: nHr++;  break;//设置闹钟小时
            case 6: nMi++;  break;//设置闹钟分钟
          }
        }; break;

      case 150452295: {//遥控器下按键
          mode2 = 4;
          switch (mode) {
            case 0: break; //彩蛋
            case 1: hr--;  break;//设置小时
            case 2: mi--;  break;//设置分钟
            case 3: s--;  break;//设置秒
            case 4: week--;  break;//设置星期
            case 5: nHr--;  break;//设置闹钟小时
            case 6: nMi--;  break;//设置闹钟分钟
          }
        }; break;

      case 150420165: {//遥控器ok按键
          mode2 = 5;
          mode = 0;
        }; break;

      case 4294967295: {//遥控器长按键
          switch (mode) {
            case 0: break; //彩蛋
            case 1: if (mode2 == 3) {
                hr++;
              } else if (mode2 == 4) {
                hr--;
              }; break; //设置小时
            case 2: if (mode2 == 3) {
                mi++;
              } else if (mode2 == 4) {
                mi--;
              }; break;//设置分钟
            case 3: if (mode2 == 3) {
                s++;
              } else if (mode2 == 4) {
                s--;
              }; break;//设置秒
            case 4: if (mode2 == 3) {
                week++;
              } else if (mode2 == 4) {
                week;
              }; break;//设置星期
            case 5: if (mode2 == 3) {
                nHr++;
              } else if (mode2 == 4) {
                nHr--;
              }; break;//设置闹钟小时
            case 6: if (mode2 == 3) {
                nMi++;
              } else if (mode2 == 4) {
                nMi--;
              }; break;//设置闹钟分钟
          }
        }; break;

    }
    irrecv.resume(); // 继续等待接收下一组信号
  }

}

void func() {
  tem = (double)analogRead(A0) * (5 / 10.23);//读取并计算温度
  if (hr == nHr && mi == nMi) {//播放闹钟1分钟
    tone(BUZZER, 200 + ms);//随毫秒数改变闹钟音调，产生铃声
  } else {
    noTone(BUZZER);//结束播放铃声
    digitalWrite(BUZZER, LOW);//设置为低电平
  }
}

void calc() {
  ms = millis() - base;
  if (ms > 999) {
    ms = 0;
    base = millis() - 61; //用于校正误差,每过1s提前61ms
    if (++s > 59) {
      s = 0;
      base = millis() + 8; //用于校正误差,每过1min延迟8ms
      if (++mi > 59) {
        mi = 0;
        base = millis() - 38; //用于校正误差,每过1hour提前38ms
        if (++hr > 23) {
          hr = 0;
          base = millis() - 8; //用于校正误差,每过1day提前8ms
          if (++week > 6) week = 0;
        }
      }
    }
  }
}

void draw() {
  if (mode == 0 && mode2 == 3) {
    u8g.drawStr(50, 24, "By");
    u8g.drawStr(16, 48, "HK-SHAO");
    return;
  }
  switch (mode) {
    case 0: {
        for (int x = 0; x < 2 * s; x++)//按照秒数绘制宽4个像素的进度条
          for (int y = 0; y < 4; y++)
            u8g.drawPixel(x + 5, y + 3);

        u8g.setFont(u8g_font_gdb30n);//设置gdb30像素数字类字体

        if (hr > 9) {//如果数字只有一位就添加0在前面
          u8g.setPrintPos( 8, 41);
        } else {
          u8g.drawStr(10, 41, "0");
          u8g.setPrintPos( 32, 41);
        }
        u8g.print(hr);

        if (s % 2 == 0)//冒号在秒数为偶数时才绘制，达到冒号闪烁的效果
          u8g.drawStr( 56, 37, ":");

        if (mi > 9) {//如果数字只有一位就添加0在前面
          u8g.setPrintPos( 70, 41);
        } else {
          u8g.drawStr(70, 41, "0");
          u8g.setPrintPos( 94, 41);
        }
        u8g.print(mi);

        u8g.setFont(u8g_font_gdb14r);//设置gdb14像素常用类字体
        switch (week) {//绘制星期数到屏幕
          case 0: u8g.drawStr(6, 61, "Sun"); break;
          case 1: u8g.drawStr(6, 61, "Mon"); break;
          case 2: u8g.drawStr(6, 61, "Tue"); break;
          case 3: u8g.drawStr(6, 61, "Wed"); break;
          case 4: u8g.drawStr(6, 61, "Thu"); break;
          case 5: u8g.drawStr(6, 61, "Fri"); break;
          default: u8g.drawStr(6, 61, "Sat");
        }

        u8g.setPrintPos( 56, 61);//绘制温度示数到屏幕
        u8g.print(tem);
        u8g.drawStr(111, 61, "C");
        break;
      }
    case 1: {
        u8g.setFont(u8g_font_gdb30n);//设置gdb30像素数字类字体
        if (hr > 9) {//如果数字只有一位就添加0在前面
          u8g.setPrintPos( 8, 41);
        } else {
          u8g.drawStr(10, 41, "0");
          u8g.setPrintPos( 32, 41);
        }
        u8g.print(hr);
        break;
      }
    case 2: {
        u8g.setFont(u8g_font_gdb30n);//设置gdb30像素数字类字体
        if (mi > 9) {//如果数字只有一位就添加0在前面
          u8g.setPrintPos( 70, 41);
        } else {
          u8g.drawStr(70, 41, "0");
          u8g.setPrintPos( 94, 41);
        }
        u8g.print(mi);
        break;
      }
    case 3: {
        for (int x = 0; x < 2 * s; x++)//按照秒数绘制宽4个像素的进度条
          for (int y = 0; y < 4; y++)
            u8g.drawPixel(x + 5, y + 3);

        u8g.setFont(u8g_font_gdb30n);//设置gdb30像素数字类字体
        if (s % 2 == 0)//冒号在秒数为偶数时才绘制，达到冒号闪烁的效果
          u8g.drawStr( 56, 37, ":");
        break;
      }
    case 4: {
        u8g.setFont(u8g_font_gdb14r);//设置gdb14像素常用类字体
        switch (week) {//绘制星期数到屏幕
          case 0: u8g.drawStr(6, 61, "Sun"); break;
          case 1: u8g.drawStr(6, 61, "Mon"); break;
          case 2: u8g.drawStr(6, 61, "Tue"); break;
          case 3: u8g.drawStr(6, 61, "Wed"); break;
          case 4: u8g.drawStr(6, 61, "Thu"); break;
          case 5: u8g.drawStr(6, 61, "Fri"); break;
          default: u8g.drawStr(6, 61, "Sat");
        }
        break;
      }
    case 5: {
        u8g.setFont(u8g_font_gdb30n);//设置gdb30像素数字类字体
        if (nHr > 9) {//如果数字只有一位就添加0在前面
          u8g.setPrintPos( 8, 41);
        } else {
          u8g.drawStr(10, 41, "0");
          u8g.setPrintPos( 32, 41);
        }
        u8g.print(nHr);
        break;
      }
    case 6: {
        u8g.setFont(u8g_font_gdb30n);//设置gdb30像素数字类字体
        if (nMi > 9) {//如果数字只有一位就添加0在前面
          u8g.setPrintPos( 70, 41);
        } else {
          u8g.drawStr(70, 41, "0");
          u8g.setPrintPos( 94, 41);
        }
        u8g.print(nMi);
        break;
      }
  }
}
