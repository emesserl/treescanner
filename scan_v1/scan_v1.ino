#include <ezLCDLib.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

File myFile;
ezLCD3 lcd;

byte mac[] = {0x90, 0xA2, 0xDA, 0x0E, 0x07, 0x8E };
IPAddress server(192, 168, 0, 10);
EthernetClient client;
int BUFFER_SIZE = 72;
boolean scanned = false;

int btnside = 40;
int x1Pos = 20;
int x2Pos = x1Pos + 5 + btnside;
int x3Pos = x2Pos + 5 + btnside;
int x4Pos = x3Pos + 5 + btnside;
int y1Pos = 20;
int y2Pos = y1Pos + 5 + btnside;
int y3Pos = y2Pos + 5 + btnside;
int y4Pos = y3Pos + 5 + btnside;
int y5Pos = y4Pos + 5 + btnside; 
char latitude[10] = {'0', '0', '.', '0', '0', '0', '0', '0', '0'};
char longitude[11] = {'0', '0', '0', '.', '0', '0', '0', '0', '0', '0'};
char azimuth[4] = {'0', '0', '0'};
int numtouches = 0;
boolean lat = false;
boolean lon = false;
boolean az = false;
int touch = 0;

void setup()
{

  lcd.begin( EZM_BAUD_RATE );
  lcd.cls(BLACK, WHITE);
  lcd.font("0");
  lcd.fontw(1, 2);
  lcd.fontw(2, 1);
  lcd.theme(1, 75, 2, 136, 3, 2, 80, 82, 1, 0, 0);//button theme
  lcd.theme(2, 0, 0, 0, 0, 1, 2, 0, 0, 3, 0);
  lcd.theme(3, 0, 0, 0, 0, 1, 3, 0, 0, 3, 0);
   lcd.string(1, "1");
  lcd.string(2, "2");
  lcd.string(3, "3");
  lcd.string(4, "4");
  lcd.string(5, "5");
  lcd.string(6, "6");
  lcd.string(7, "7");
  lcd.string(8, "8");
  lcd.string(9, "9");
  lcd.string(10, "0");
  lcd.string(11, "Clear");
  lcd.string(12, "Next");
  lcd.string(13, "Latitude");
  lcd.string(14, "Longitude");
  lcd.string(15, "Azimuth");
  lcd.string(16, "Scan");
  lcd.string(17, "Exit");
  lcd.string(18, "00.000000");
  lcd.string(19, "000.000000");
  lcd.string(20, "000");
  lcd.string(21, "Scan Success");
  lcd.string(22, "Quit");
}

void loop()
{
  if(lat)
  {
    numtouches = 0;
    enter_latitude();
    while(lat)
    {
      capture_touches(latitude, 18, 11, 9);
    }
  }
  else if(lon)
  {
    numtouches = 0;
    enter_longitude();
    while(lon){capture_touches(longitude, 19, 11, 10);}
  }
  else if(az){
    numtouches = 0; 
    enter_azimuth();
    while(az){capture_touches(azimuth, 20, 11, 3);}
  }
  else
  {
    final_screen();
    while(!az && !lat && !lon)
    {
      final_touches();
    }
  }
}

void capture_touches(char *data, int str, int txt, int maxtouches)
{
  touch = lcd.wstack(0);
  if(lcd.currentInfo == 4){
  if(touch == 13){if(lat){lat = false; lon = true; az = false;}
  else if(lon){lon = false; az = true; lat = false;}else if(az){lon = false; lat = false; az = false;}}
  if(touch == 14){lat = false;lon = false;az = false;}
  if(touch == 12){ clear_data(data, str, txt);}
  if(touch > 0 && touch < 11)
  {
    if(numtouches < maxtouches && lcd.currentInfo == 4)
    {
      enter_data(data, touch);
      update_text(data, str, txt);
      if(lat)
      {
        if(numtouches == 1){numtouches = 3;}
        else numtouches++;
      }
      else if(lon)
      {
        if(numtouches == 2){numtouches = 4;}
        else numtouches++;
      }
      else {numtouches++;}
    }
  }
  }
  //lcd.wstack(2);
}

void update_text(char *data, int strchange, int txtchange)
{
  lcd.string(strchange, data);
  lcd.wstate(txtchange, 3);
}

void clear_data(char *data, int strchange, int txtchange)
{
  if(lat){
  lcd.string(strchange, "00.000000");}
  else if(lon){lcd.string(strchange, "000.000000");}
  else{lcd.string(strchange, "000");}
  lcd.wstate(txtchange, 3);
  numtouches = 0;
  int dec = 2;
  int maxcnt = 9;
  if(lon){dec = 3;maxcnt = 10;}
  if(az){dec = 4; maxcnt = 3;}
  for(int i = 0;i<maxcnt;i++)
  {
    if(i==dec) data[i] = '.'; else data[i] = '0'; 
  }
  
}

void enter_data(char *data, int tnum)
{
  if(tnum == 1){data[numtouches] = '1';}
  if(tnum == 2){data[numtouches] = '2';}
  if(tnum == 3){data[numtouches] = '3';}
  if(tnum == 4){data[numtouches] = '4';}
  if(tnum == 5){data[numtouches] = '5';}
  if(tnum == 6){data[numtouches] = '6';}
  if(tnum == 7){data[numtouches] = '7';}
  if(tnum == 8){data[numtouches] = '8';}
  if(tnum == 9){data[numtouches] = '9';}
  if(tnum == 10){data[numtouches] = '0';}
}

void enter_azimuth()
{
  lcd.cls(BLACK, WHITE);
  initialize_widgets(20, 15);
}

void enter_longitude()
{
  lcd.cls(BLACK, WHITE);
  initialize_widgets(19, 14);
}

void enter_latitude()
{
  lcd.cls(BLACK, WHITE);
  initialize_widgets(18, 13);
}

void initialize_widgets(int dtype, int txtype)
{
  lcd.staticText(11, x1Pos, y1Pos, 150, 20, 3, 2, dtype);
  lcd.staticText(15, x1Pos + 155, y1Pos, 115, 20, 1, 3, txtype);
  lcd.button(1, x1Pos, y2Pos, btnside, btnside, 1, 0, 0, 1, 1);
  lcd.button(2, x2Pos, y2Pos, btnside, btnside, 1, 0, 0, 1, 2);
  lcd.button(3, x3Pos, y2Pos, btnside, btnside, 1, 0, 0, 1, 3);
  lcd.button(4, x1Pos, y3Pos, btnside, btnside, 1, 0, 0, 1, 4);
  lcd.button(5, x2Pos, y3Pos, btnside, btnside, 1, 0, 0, 1, 5);
  lcd.button(6, x3Pos, y3Pos, btnside, btnside, 1, 0, 0, 1, 6);
  lcd.button(7, x1Pos, y4Pos, btnside, btnside, 1, 0, 0, 1, 7);
  lcd.button(8, x2Pos, y4Pos, btnside, btnside, 1, 0, 0, 1, 8);
  lcd.button(9, x3Pos, y4Pos, btnside, btnside, 1, 0, 0, 1, 9);
  lcd.button(10, x2Pos, y5Pos, btnside, btnside, 1, 0, 0, 1, 10);
  lcd.button(12, x4Pos, y2Pos, 70, btnside, 1, 0, 0, 1, 11);
  lcd.button(13, x4Pos, y3Pos, 60, btnside, 1, 0, 0, 1, 12);
  lcd.button(14, x4Pos, y4Pos, 60, btnside, 1, 0, 0, 1, 17);
}

void final_screen()
{
  lcd.cls(BLACK, WHITE);
  lcd.staticText(1, x1Pos, y1Pos, 150, 20, 3, 2, 18);
  lcd.staticText(2, x1Pos + 155, y1Pos, 115, 20, 1, 3, 13);
  lcd.staticText(3, x1Pos, y1Pos + 25, 150, 20, 3, 2, 19);
  lcd.staticText(4, x1Pos + 155, y1Pos + 25, 115, 20, 1, 3, 14);
  lcd.staticText(5, x1Pos, y1Pos + 50, 150, 20, 3, 2, 20);
  lcd.staticText(6, x1Pos + 155, y1Pos + 50, 115, 20, 1, 3, 15);
  //lcd.button(7, x1Pos, y3Pos, 60, btnside, 1, 0, 0, 1, 17);
  lcd.button(7, x4Pos, y3Pos, 60, btnside, 1, 0, 0, 1, 16);
  lcd.button(8, x1Pos, y3Pos, 115, btnside, 1, 0, 0, 1, 13);
  lcd.button(9, x1Pos, y4Pos, 115, btnside, 1, 0, 0, 1, 14);
  lcd.button(10, x1Pos, y5Pos, 115, btnside, 1, 0, 0, 1, 15);
  lcd.staticText(11, x4Pos, y4Pos, 150, 20, 3, 2, 21);
  lcd.button(12, x4Pos, y5Pos, 60, btnside, 1, 0, 0, 1, 22);
}

void final_touches()
{
  touch = lcd.wstack(0);
  if(lcd.currentInfo == 4)
  {
    if(touch == 8){lat = true;}
    if(touch == 9){lon = true;}
    if(touch == 10){az = true;}
    if(touch == 7){int err = 0; err = scan_plot(); if(myFile){myFile.println(""); myFile.close();}
      if(err <= 0){char erra[1]; itoa(err, erra, 10); lcd.string(21, erra);lcd.wstate(11, 3);}
      else {
      lcd.string(21, "Scan Succeeded"); 
      lcd.wstate(11, 3);
    }
    }//begin scan routine
    if(touch == 12){ if(myFile){myFile.println(""); myFile.close();}}//quit
  }
}

int next_plot()
{
  int start = 1;
  if(send_distance_command())
    {
      int cnt = 0;
      while(start > 0 && cnt < 10)
      {start = urg_get_distance(); myFile.println(""); myFile.println(""); cnt++;}
    }
    else start = -4;
    return start;
}

int scan_plot()
{
  lcd.string(21, "Scanning");lcd.wstate(11, 3);
  int start = 1;
  if(!myFile){
     start = begin_scan();}
  if(start <= 0) return start;
  if(start_urg_device())
  {
    start = next_plot();
  }
  else start = -3;
  ignore_receive_data();
  scip_response("QT\n");
  return start;
}

int begin_scan()
{
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  if(!SD.begin(4)){return -1;}else{myFile = SD.open("scan.txt", FILE_WRITE);}

  Ethernet.begin(mac, server);
  delay(1000);
  if(!client.connect(server, 10940)){if(myFile){myFile.close(); }return -2;}
    if(myFile)
  {
    myFile.println("Latitude");myFile.println(latitude);
    myFile.println("Longitude"); myFile.println(longitude);
    myFile.println("Azimuth"); myFile.println(azimuth);
  }
  return 1;
}

boolean start_urg_device()
{
  if(!scip_response("QT\n")){return false;}
  if(!scip_response("SCIP2.0\n")){return false;}
  return true;
}

boolean send_distance_command()
{
  char buffer[BUFFER_SIZE];
  boolean good = true;
  client.print("MD0000108001000\n");
  int n = read_laser(buffer, BUFFER_SIZE);  
  if(n<= 0){n = read_laser(buffer, BUFFER_SIZE);}
  if(strncmp(buffer, "MD", 2)) { ignore_receive_data_with_qt(); return false; }
  n = read_laser(buffer, BUFFER_SIZE);
  if(!strncmp(buffer, "00", 2))
  {
    return true;
  }
  return false;
}

int urg_get_distance()
{
  char dbuffer[BUFFER_SIZE];
  int ret = 0;
  int n;
  //read the first line this tells us what type it is, MD
  n = read_laser(dbuffer, BUFFER_SIZE);
  if(n<=0)
  {
    n = read_laser(dbuffer, BUFFER_SIZE);
  }
  if(strncmp(dbuffer, "MD", 2))
  {
    ignore_receive_data_with_qt();
    return -1;
  }
  //read the second line, this one tells us whether we are good
  n = read_laser(dbuffer, BUFFER_SIZE);
  if(n > 0)
  {
    if(!strncmp(dbuffer, "99", 2))
    {
      //read time stamp
      n = read_laser(dbuffer, BUFFER_SIZE);
      if(n > 0){
      return receive_length_data();
    }
    }
  }
  return -1;
}

int receive_length_data()
{
  int n;
  long length[72];
  char buffer[72];
  int step_filled = 0;
  int line_filled = 0;
  int data_size = 3;
  int count = 0;

  do{
    //step_filled = 0;
    char *p = buffer;
    char *last_p;
    n = client.readBytesUntil('\n', &buffer[line_filled], 72 - line_filled);
    //n = read_laser(&buffer[line_filled], 72 - line_filled, true);//typically reads 65
    //if(n <= 0){ return -1;}
    if(n > 0)
    {
      
      if(buffer[line_filled + n - 1] != scip_checksum(&buffer[line_filled], n - 1))
      {
        return -1;
      }
      line_filled += n - 1;
    }
    last_p = p + line_filled;

    while((last_p - p) >= data_size)
    {
      int index;
      if(*p == '&')
      {
        if((last_p - (p + 1)) < data_size)
        {
          break;
        }
        --step_filled;
        ++p;
        --line_filled;
      }

      index = step_filled;
      if(step_filled > (1080 - 0))//need to send the QT command
      {
        ignore_receive_data();
        return -1;
      }
      if(length)
      {
        length[index] = urg_scip_decode(p, 3);
      }
      p += 3;
      ++step_filled;
      line_filled -= data_size;
    }
    if(step_filled > 0){write_data(length, step_filled); step_filled = 0;}
    count++;
  }while(n > 0);
  return 1;
}

long urg_scip_decode(const char data[], int bsize)
{
  const char* p = data;
  const char* last_p = p + bsize;
  long value = 0;
  while(p < last_p)
  {
    value <<= 6;
    value &= ~0x3f;
    value |= *p++ - 0x30;
  }
  return value;
}

char scip_checksum(const char buffer[], int bsize)
{
  unsigned char sum = 0x00;
  int i;
  for(i = 0;i<bsize;i++)
  {
    sum += buffer[i];
  }
  return (sum & 0x3f) + 0x30;
}

boolean scip_response(char *command)
{
  char rbuf[72];
  client.print(command);
  int n = read_laser(rbuf, 72);
  if(n <= 0){n = read_laser(rbuf, 72);}
  if(n <= 0){ return false; }
  if(strncmp(rbuf, command, 2)){return false;}
  if(command == "SCIP2.0\n"){read_laser(rbuf, 72); return true;}
  n = read_laser(rbuf, 72);
  if(!strncmp(rbuf, "00", 2)){return true;}
  return false;
}

int read_laser(char *data, int maxsize)
{
  int i = 0;
    for(;i<maxsize;++i)
    {
      char ch = client.read();
      if(ch <= 0){ 
        --i;//error
        continue;
      }
      if(ch == '\n' || ch == '\r')
      {
        break;
      }
      data[i] = ch;
    }
    if(i >= maxsize)
    {
      --i;
      data[maxsize - 1] = '\0';
    }
    else{
    data[i] = '\0';}
    return i;
}

int read_line(char *data, int maxsize)
{
  int i = 0;
  //if(client.connected())
  //{
    boolean isNL2 = false;
    boolean isNL = false;
    for(i = 0;i<maxsize;i++)
    {
      char buf = client.read();
      if(buf == '\n')
      {
        if(isNL)
        {
          isNL2 = true;
        }
        else
        {
          isNL = true;
        }
      }
      else
      {
        isNL = false;
      }
      data[i] = buf;
      if(isNL2)
      {
        break;
      }
    }
  //}
  if(i >= maxsize)
    {
      data[maxsize - 1] = '\0';
    }
    else{
    data[i] = '\0';}
  return i;
}

void ignore_receive_data_with_qt()
{
  scip_response("QT\n");
  ignore_receive_data();
  return;
}

void ignore_receive_data()
{
  char buffer[BUFFER_SIZE];
  int n = read_line(buffer, BUFFER_SIZE);
  int count = 0;
  while(n > 0 && count < 25){n = read_line(buffer, BUFFER_SIZE); count++;}
  //if(!scip_response("QT\n")){scip_response("QT\n");}
}

void write_data(long *data, int maxsize)
{
  if(!myFile){ return;}
  for(int i = 0;i<maxsize;i++)
    {
      char len[10];
      ltoa(data[i], len, 10);
      myFile.print(len);
      myFile.print("|");
    }
}
