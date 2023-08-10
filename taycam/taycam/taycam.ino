#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F,20,4);
RF24 radio(9, 10);// CE, CSN
const byte diachi[][6] = {"12345", "10000"};

  /*
  * nrf24l01: IRQ-D8 CE-D9 CSN-D10 MOSI-D11 MISO-D12 CLK-13
  * button: A~D2 B~D3 C~D4 D~D5 E~D6 F~D7 X~A0 Y~A1 Key_Joystick~D8
  */
#define x A0
#define y A1
#define joy 8
#define a 2
#define enter 3 
#define c 4
#define d 5 
#define e 6
#define f 7 
int gt_x; int gt_y;
byte gt_a; byte gt_b; byte gt_c; byte gt_joy;byte gt_d;
byte gt_len; byte gt_xuong; byte gt_enter;byte gt_them;
int dem_menu=0; int dem_lenxuong=0;
int macdinh1 = 1; int macdinh2 = 1; int macdinh3 = 1;int macdinh4 = 1;
unsigned int vitri=1;
byte guitx[30]; //0-255
byte nhantx[30];
unsigned int diachi_A=6; unsigned int diachi_B=7; unsigned int diachi_Z=8; unsigned int diachi_Kep=9;
unsigned int diachi_cuoi;

int batdau=10;
int ketthuc;

unsigned long time=0;
unsigned long dem_chay = 1; boolean run;
int solanchay=0;
byte comandnumber = 1;

void setup() 
{
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();  
  man_hinh();

  
  pinMode(x, INPUT);
  pinMode(y, INPUT);
  pinMode(joy, INPUT_PULLUP);
  pinMode(a, INPUT_PULLUP);
  pinMode(enter, INPUT_PULLUP); //Auto home
  pinMode(c, INPUT_PULLUP);
  pinMode(d, INPUT_PULLUP);
  if (!radio.begin()) 
  {
    Serial.println("Module không khởi động được...!!");
    while (1) {}
  }   

  radio.openWritingPipe(diachi[1]);
  radio.openReadingPipe(1, diachi[0]);
  radio.setPALevel(RF24_PA_MIN); //Cài bộ khuyết địa công suất ở mức MIN
  radio.setChannel(80);
  radio.setDataRate(RF24_250KBPS);

  if (!radio.available()) //khong ket noi
  {
    Serial.println("Chưa kết nối được với RX...!!");
    Serial.println("CHỜ KẾT NỐI.......");
  } 
}

void loop() 
{
  gt_enter = digitalRead(enter); 
  gt_len = digitalRead(a);
  gt_xuong = digitalRead(c);
  gt_them= digitalRead(d);
  if(gt_enter != macdinh1)
  {
     if(gt_enter == 0) dem_menu+=1;
     if(dem_menu == 1) menu();
     if(dem_menu == 2 && dem_lenxuong == 0) //Chon BACK ve man hinh
     {
        lcd.clear();
        man_hinh();
        dem_menu = 0; dem_lenxuong = 0;
     }
     if(dem_menu == 2 && dem_lenxuong == 1) //Chon them lenh moi
      {
        Them_lenh();
      }
      if(dem_menu == 3 && dem_lenxuong == 1) //Thoat khoi THEM LENH
      {
        EEPROM.write(1,1); //Luu so 1 vao o 1 de xac nhan da co lenh duoc luu
        diachi_cuoi = diachi_Kep;
        EEPROM.write(2, diachi_cuoi); //Luu dia chi cuoi cung vao o so 2
        // Serial.println(EEPROM.read(2));
        
        lcd.clear();
        man_hinh();
        dem_menu = 0;
        dem_lenxuong = 0;
        vitri = 1;
        diachi_A = 6; diachi_B = 7; diachi_Z = 8; diachi_Kep = 9;
      }

     if(dem_menu == 2 && dem_lenxuong == 2) //Chon xoa lenh
      {
        xoa_lenh();
      }
     if(dem_menu == 2 && dem_lenxuong == 3) //Chon CHAY LENH
      {
        if (KT_co_lenh() == true)
        {
            them_sl_chay();
          }
        //run = true;
        
       
      }
      if(dem_menu == 3 && dem_lenxuong == 3)
      {
         run = true;
      }
      if(dem_menu == 2 && dem_lenxuong == 4) //AUTOHOME
      {
        lcd.clear();
        dk_servo(0);
        man_hinh();
        dem_menu = 0;
        dem_lenxuong = 0;
      }
      if(dem_menu == 4 && dem_lenxuong == 3)
      {
        run = false;
        batdau = 10;
        ketthuc = 0;
        dem_chay = 0; dem_menu = 0; dem_lenxuong = 0; 
        lcd.clear();
        solanchay=0;
        man_hinh();          
      }
     macdinh1 = gt_enter;
  } 
  if(gt_len != macdinh2) //NUT LEN
  {
    if(gt_len == 0 && dem_menu == 1) //Di chuyen o MENU
    {
      dem_lenxuong-=1;
      if(dem_lenxuong<0)  dem_lenxuong=4;
      menu();
    }
    if(gt_len == 0 && dem_menu == 2 && dem_lenxuong == 3) //Di chuyen o cahy lenh
    { 
      solanchay++;
     if (solanchay == 0)
      {
        lcd.setCursor(14,2);
        lcd.print("ALWAYS");
      }
      else
      if (solanchay <10 )
      {
      lcd.setCursor(14,2);
      lcd.print(solanchay);
      lcd.setCursor(15,2);
      lcd.print("      ");
      }
      else
      {
        lcd.setCursor(14,2);
        lcd.print(solanchay);
        lcd.setCursor(16,2);
        lcd.print("      ");
      }
    }
    macdinh2 = gt_len;
  }
  if (gt_them !=  macdinh4)
  {
    if(gt_them == 0 && dem_menu == 2) //Neu o MENU them lenh thi nhan nut len de them VI TRI cua lenh
    {      
      luu_lenh();
      vitri += 1;
      Serial.println(vitri);
       lcd.setCursor(12,1);
      lcd.print(vitri);   
      
      // Serial.print("DC A: "); Serial.print(diachi_A); Serial.print("  - DC B: "); Serial.print(diachi_B);
      // Serial.print("  - DC Z: "); Serial.print(diachi_Z); Serial.print("  - DC KEP: "); Serial.println(diachi_Kep);

      // Xem_lenh();
    }
    
    macdinh4 = gt_them;
  } 
 
  if(gt_xuong != macdinh3) //NUT XUONG
  {
    if(gt_xuong == 0 && dem_menu == 1) //Di chuyen o MENU
    {
      dem_lenxuong+=1;
      if(dem_lenxuong>4) dem_lenxuong=0;
      menu();
    }
    if(gt_xuong == 0 && dem_menu == 2 && dem_lenxuong == 3 && solanchay !=0) //Di chuyen o cahy lenh
    {
      solanchay--;
      if (solanchay == 0)
      {
        lcd.setCursor(14,2);
        lcd.print("ALWAYS");
      }
      else
      if (solanchay <10 )
      {
      lcd.setCursor(14,2);
      lcd.print(solanchay);
      lcd.setCursor(15,2);
      lcd.print("      ");
      }
      else
      {
        lcd.setCursor(14,2);
        lcd.print(solanchay);
        lcd.setCursor(16,2);
        lcd.print("      ");
      }
    }
    macdinh3 = gt_xuong;
  }
  if(dem_menu == 0 && dem_lenxuong == 0) //Tai MAN HINH
  {
    dk_servo(1);
    HT_goc(0);
    hienthikep();
  } 
  else if(dem_menu == 2 && dem_lenxuong == 1) //Tai MENU THEM LENH
  {
    dk_servo(1);
    HT_goc(2);
    hienthikep();
  }  
  if(run == true && dem_menu == 3 && dem_lenxuong == 3) //CHAY LENH
  {
    chay_lenh();
    //Serial.println(dem_menu); 
    //Serial.println(dem_lenxuong); 
  }
}
void man_hinh()
{
  
  if ((dem_menu==0 &&  dem_lenxuong == 0) || (dem_menu==4 &&  dem_lenxuong == 3) || (dem_menu==3 &&  dem_lenxuong == 1) ||(dem_menu == 2 && dem_lenxuong == 0) || (dem_menu == 2 && dem_lenxuong == 4))
  {
    lcd.setCursor(1,1);
    lcd.print("LENH: OFF ");
    lcd.setCursor(1,2);
    lcd.print("DIEU KHIEN: ON ");
  }
  else
  if (dem_menu==3 &&  dem_lenxuong == 3)
  {
    lcd.setCursor(1,1);
    if (solanchay !=0 )
    {
      lcd.print("LENH: ON - SL: ");
      lcd.print(solanchay);
    } else
    {
      lcd.print("LENH: ON - SL: ");
      lcd.print("ALS");
    }
    
    lcd.setCursor(1,2);
    lcd.print("DIEU KHIEN: OFF");
    lcd.setCursor(4,3);
    lcd.print("[ OK ] -> STOP");
  }
  
}
void HT_goc(int hang)
{
  lcd.setCursor(1,hang); lcd.print("A:");
  lcd.setCursor(3,hang); lcd.print(nhantx[2]);

  if(nhantx[3]<100)
  {
    lcd.setCursor(7,hang); lcd.print("B:");
    lcd.setCursor(9,hang); lcd.print(nhantx[3]);
    lcd.setCursor(11,hang); lcd.print(" ");
  }
  else 
  {
    lcd.setCursor(7,hang); lcd.print("B:");
    lcd.setCursor(9,hang); lcd.print(nhantx[3]);   
  }

  if(nhantx[1] < 10)
  {
    lcd.setCursor(13,hang); lcd.print("Z:");
    lcd.setCursor(15,hang); lcd.print(nhantx[1]);
    lcd.setCursor(16,hang); lcd.print("  ");
  }
  else if(nhantx[1] < 100)
  {
    lcd.setCursor(13,hang); lcd.print("Z:");
    lcd.setCursor(15,hang); lcd.print(nhantx[1]);
    lcd.setCursor(17,hang); lcd.print(" ");
  }  
  else 
  {
    lcd.setCursor(13,hang); lcd.print("Z:");
    lcd.setCursor(15,hang); lcd.print(nhantx[1]);
  }
}


void hienthikep()
{
  if (nhantx[4]==180)
  {
    lcd.setCursor(5,3); lcd.print("KEP: Dong");
      }
  else {
    lcd.setCursor(5,3); lcd.print("KEP: Mo");
     lcd.setCursor(12,3); lcd.print("  ");

  }
}
void menu()
{
  if (dem_lenxuong == 0)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(">BACK");
    lcd.setCursor(0,1);
    lcd.print(" THEM LENH");
    lcd.setCursor(0,2);
    lcd.print(" XOA LENH");  
    lcd.setCursor(0,3);
    lcd.print(" CHAY LENH");
  }
  else if (dem_lenxuong == 1)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" BACK");
    lcd.setCursor(0,1);
    lcd.print(">THEM LENH");
    lcd.setCursor(0,2);
    lcd.print(" XOA LENH");  
    lcd.setCursor(0,3);
    lcd.print(" CHAY LENH");     
  }
  else if(dem_lenxuong == 2)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" BACK");
    lcd.setCursor(0,1);
    lcd.print(" THEM LENH");
    lcd.setCursor(0,2);
    lcd.print(">XOA LENH");  
    lcd.setCursor(0,3);
    lcd.print(" CHAY LENH");    
  }
  else if(dem_lenxuong == 3)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" BACK");
    lcd.setCursor(0,1);
    lcd.print(" THEM LENH");
    lcd.setCursor(0,2);
    lcd.print(" XOA LENH");  
    lcd.setCursor(0,3);
    lcd.print(">CHAY LENH");    
  }
  else if (dem_lenxuong == 4)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" THEM LENH");
    lcd.setCursor(0,1);
    lcd.print(" XOA LENH");
    lcd.setCursor(0,2);
    lcd.print(" CHAY LENH");  
    lcd.setCursor(0,3);
    lcd.print(">VE NHA"); 
  }
}
void dk_servo(int m)
{
  guitx[0]=0;
  guitx[1] = m;
  //Serial.print("DC A: "); Serial.print(guitx[1]);
  //Serial.println();
  gt_a = digitalRead(a);
  guitx[4] = gt_a;
  gt_c = digitalRead(c);
  guitx[5] = gt_c;
  gt_joy = digitalRead(joy);
  guitx[6] = gt_joy;

  gt_x = analogRead(x); //X
  guitx[2] = map(gt_x, 0, 1023, 0, 100);
  gt_y = analogRead(y); //Y
  guitx[3] = map(gt_y, 0, 1023, 0, 100);
  //gửi tín hiệu
  radio.stopListening();
  radio.write(&guitx, sizeof(guitx)); //Gui du lieu  
  delay(10);

  
  //nhận tín hiệu
  radio.startListening();
  while(!radio.available());
  radio.read(&nhantx, sizeof(nhantx));
  delay(10);
}

bool KT_co_lenh()
{
  boolean co;
  if(EEPROM.read(1) == 1) //Da co lenh duoc luu
    co = true;        
  else
    co = false;
  return co;
}
void Them_lenh()
{
  
  if(KT_co_lenh() == true) //Da co lenh duoc luu
  {
    lcd.clear();
    lcd.setCursor(3,1);
    lcd.print("DA CO LENH...!"); 
    delay(3000);
    lcd.clear();
    dem_menu = 0; dem_lenxuong = 0;
     man_hinh();         
  }
  else //Chua co lenh nao duoc luu
  {
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("XIN MOI NHAP LENH ");
    lcd.setCursor(4,1);
    lcd.print("VI TRI: ");
    lcd.setCursor(12,1);
    lcd.print(vitri);          
  }  
}
void them_sl_chay()
{
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("NHAP SO LAN CHAY");
    lcd.setCursor(4,2);
    lcd.print("SO LUONG: ");
    if (solanchay == 0)
    {
      lcd.setCursor(14,2);
       lcd.print("ALWAYS");
    }
    else
    {
      lcd.setCursor(14,2);
      lcd.print(solanchay);
    }
        
}
void xoa_lenh()
{
  if(KT_co_lenh() == true) //Co LENH
  {
    lcd.clear();
    lcd.setCursor(2,1);
    lcd.print("DANG XOA LENH...");        
    lcd.setCursor(4,2);
    lcd.print("VUI LONG CHO!"); 

    dem_menu = 0; dem_lenxuong = 0;

    for (int i = 0 ; i < EEPROM.length() ; i++) 
    {
      EEPROM.write(i, 0);
    } 

    // Xem_lenh();

    delay(3000);

    lcd.clear();
    man_hinh();
  }
  else //Hien khong co lenh trong EEPROM
  {
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("KHONG CO LENH NAO");
    lcd.setCursor(4,2);
    lcd.print("DUOC LUU...!");   

    dem_menu = 0; dem_lenxuong = 0;

    delay(3000);
    lcd.clear();
    man_hinh();    
  }

}
void luu_lenh()
{ 
  //6,7,8,9 ; 10,11,12,13 ; 14,15,16,17 ; 18,19,20,21
  diachi_A += 4;
  diachi_B += 4;
  diachi_Z += 4;
  diachi_Kep += 4;

  EEPROM.write(diachi_A, nhantx[2]);
  EEPROM.write(diachi_B, nhantx[3]);
  EEPROM.write(diachi_Z, nhantx[1]);
  EEPROM.write(diachi_Kep, nhantx[4]);
  delay(50);
}
void chay_lenh()
{
  if(KT_co_lenh() == true) //Neu co lenh
  { 
    lcd.clear();
    man_hinh();    
    byte doc_A; byte doc_B; byte doc_Z; byte doc_kep;
    int so_lan_da_chay=0;
    ketthuc = EEPROM.read(2); //doc o nho cuoi cung
    //Serial.println(solanchay);
    //Serial.println(run);
    while(run == true)
    {
      time = millis();
      gt_enter = digitalRead(enter);
      //Serial.println(dem_menu); 
      //Serial.println(dem_lenxuong); 
      Serial.println(gt_enter); 
      if(gt_enter == 1 && dem_menu==3 && dem_lenxuong == 3)
      {
        //if (solanchay==0) so_lan_da_chay=1;
        //Serial.println(so_lan_da_chay); 
        //Serial.println(dem_menu); 
        //Serial.println(dem_lenxuong); 
        
        while(batdau < ketthuc)
        {
          gt_enter = digitalRead(enter);
          if(millis() - time > 1000 && gt_enter == 1)
          {
            HT_goc(0);
            guitx[0]=1;
            guitx[1] = EEPROM.read(batdau);
            guitx[2] = EEPROM.read(batdau + 1);
            guitx[3] = EEPROM.read(batdau + 2);
            guitx[4] = EEPROM.read(batdau + 3);
            //Serial.print("nhan: "); Serial.print(guitx[0]);
            //Serial.print("--z: "); Serial.print(guitx[1]); Serial.print(" --b "); Serial.print( guitx[2]);
            //Serial.print("--c: "); Serial.print(guitx[3]); Serial.print(" --kep"); Serial.print(guitx[4]);
            Serial.println(); 
            //gửi tín hiệu
            radio.stopListening();
            radio.write(&guitx, sizeof(guitx)); //Gui du lieu  
            delay(10);

  
            //nhận tín hiệu
            radio.startListening();
            while(!radio.available());
            radio.read(&nhantx, sizeof(nhantx));
            delay(10);
            time = millis();
            batdau += 4;              
          }
          
          else if(gt_enter == 0)
          {
            run = false;
            break;
          } 
          else if (so_lan_da_chay >= solanchay && solanchay !=0 )
          {
          run = false;      
          break;
          }
          //Serial.println(dem_menu); 
          //Serial.println(dem_lenxuong); 
        }
        
        //Serial.println(so_lan_da_chay); 
      }
      else if(gt_enter == 0 && dem_menu==3 && dem_lenxuong == 3)
      {
        run = false;        
        break;
      }
      if (so_lan_da_chay >= solanchay && solanchay !=0 )
      {
        run = false;
        ketthuc = 0;
        dem_chay = 0; dem_menu = 0; dem_lenxuong = 0; 
        lcd.clear();
        solanchay=0;
        man_hinh();   
        //Serial.println(so_lan_da_chay); 
        break;
      }
      batdau = 10;
      so_lan_da_chay++;
      Serial.print(so_lan_da_chay); 
      Serial.println(solanchay); 
    }
   }
       
  else //KHONG CO LENH NAO DUOC LUU
  {
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("KHONG CO LENH NAO");
    lcd.setCursor(4,2);
    lcd.print("DUOC LUU...!");   

    dem_menu = 0; dem_lenxuong = 0;

    delay(3000);
    lcd.clear();
    man_hinh();       
  }
}
