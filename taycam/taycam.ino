#include <Servo.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F,20,4);

/*
* Servo A-8; B-9; Z-10; Kep-11
* Bien tro: A-A0; B-A1; Z-A2 
* Nut nhan - 12
*
* Menu: len-7; xuong-6; enter-5
*/

#define enter 3
#define len 2
#define xuong 4
byte gt_len; byte gt_xuong; byte gt_enter;
int dem_menu=0; int dem_lenxuong=0;
int macdinh1 = 1; int macdinh2 = 1; int macdinh3 = 1;

Servo servo_A;
Servo servo_B;
Servo servo_Z;
Servo servo_kep;

#define servoA  8
#define servoB  9
#define servoZ  10 
#define servokep  11

#define nutnhan 12
#define bientro_A A0
#define bientro_B A1
#define bientro_Z A2

int gt_bientro_A; int gt_bientro_B; int gt_bientro_Z;
int goc_A;  int goc_B;  int goc_Z;
int gt_nutnhan; 
int biendem = 0;
int macdinh = 1; 

unsigned int vitri=1;

unsigned int diachi_A=6; unsigned int diachi_B=7; unsigned int diachi_Z=8; unsigned int diachi_Kep=9;
unsigned int diachi_cuoi;

int batdau=10;
int ketthuc;

unsigned long time=0;
unsigned long dem_chay=1; boolean run;
void setup() 
{
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();  
  man_hinh();

  servo_A.attach(servoA);
  servo_B.attach(servoB);
  servo_Z.attach(servoZ);
  servo_kep.attach(servokep);

  pinMode(bientro_A, INPUT);
  pinMode(bientro_B, INPUT);
  pinMode(bientro_Z, INPUT);
  pinMode(nutnhan, INPUT_PULLUP);

  pinMode(len, INPUT_PULLUP);
  pinMode(xuong, INPUT_PULLUP);
  pinMode(enter, INPUT_PULLUP);
}

void loop() 
{
  gt_nutnhan = digitalRead(nutnhan);
  gt_enter = digitalRead(enter); 
  gt_len = digitalRead(len);
  gt_xuong = digitalRead(xuong);
  
  if(gt_enter != macdinh1) //Nut ENTER
  {
    if(gt_enter == 0) dem_menu+=1;

      if(dem_menu == 1) menu(); //Vao menu chinh

      if(dem_menu == 2 && dem_lenxuong == 0) //Chon BACK ve man hinh
      {
        lcd.clear();
        man_hinh();
        dem_menu = 0; dem_lenxuong = 0;
      }

      if(dem_menu == 2 && dem_lenxuong == 2) //Chon xoa lenh
      {
        xoa_lenh();
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

      if(dem_menu == 2 && dem_lenxuong == 3) //Chon CHAY LENH
      {
        run = true;
      }

      if(dem_menu == 3 && dem_lenxuong == 3)
      {
        run = false;
        batdau = 10;
        ketthuc = 0;
        dem_chay = 0; dem_menu = 0; dem_lenxuong = 0; 
        lcd.clear();
        man_hinh();          
      }

    macdinh1 = gt_enter;
  }

  if(gt_len != macdinh2) //NUT LEN
  {
    if(gt_len == 0 && dem_menu == 1) //Di chuyen o MENU
    {
      dem_lenxuong-=1;
      if(dem_lenxuong<0)  dem_lenxuong=3;
      menu();
    }

    if(gt_len == 0 && dem_menu == 2) //Neu o MENU them lenh thi nhan nut len de them VI TRI cua lenh
    {      
      luu_lenh();
      vitri += 1;
      lcd.setCursor(12,1);
      lcd.print(vitri);      
      
      // Serial.print("DC A: "); Serial.print(diachi_A); Serial.print("  - DC B: "); Serial.print(diachi_B);
      // Serial.print("  - DC Z: "); Serial.print(diachi_Z); Serial.print("  - DC KEP: "); Serial.println(diachi_Kep);

      // Xem_lenh();
    }

    macdinh2 = gt_len;
  } 
 
  if(gt_xuong != macdinh3) //NUT XUONG
  {
    if(gt_xuong == 0 && dem_menu == 1) //Di chuyen o MENU
    {
      dem_lenxuong+=1;
      if(dem_lenxuong>3) dem_lenxuong=0;
      menu();
    }
    macdinh3 = gt_xuong;
  }

  if(dem_menu == 0 && dem_lenxuong == 0) //Tai MAN HINH
  {
    dk_servo();
    HT_goc(0);
  } 
  else if(dem_menu == 2 && dem_lenxuong == 1) //Tai MENU THEM LENH
  {
    dk_servo();
    HT_goc(2);
  }  
  
  if(run == true && dem_menu == 2 && dem_lenxuong == 3) //CHAY LENH
  {
    chay_lenh();
  }

  // Serial.print("BT_A: "); Serial.print(gt_bientro_A); Serial.print(" - GOC A: "); Serial.print(goc_A);
  // Serial.print("  | BT_B: "); Serial.print(gt_bientro_B); Serial.print(" - GOC B: "); Serial.print(goc_B);
  // Serial.print("  | BT_Z: "); Serial.print(gt_bientro_Z); Serial.print(" - GOC Z: "); Serial.print(goc_Z);
  // Serial.print("  | biendem: "); Serial.print(biendem);

  Serial.print("Len: "); Serial.print(gt_len); Serial.print("  - Xuong: "); Serial.print(gt_xuong); Serial.print("  - Enter: "); Serial.print(gt_enter);
  Serial.print(" | Dem_MENU: "); Serial.print(dem_menu); Serial.print(" - Dem_LX: "); Serial.print(dem_lenxuong);
  Serial.print(" - RUN: "); Serial.print(run);
  Serial.print(" - BatDau: "); Serial.print(batdau); Serial.print(" - KetThuc: "); Serial.print(ketthuc); 
  Serial.print(" - TIME: "); Serial.print(time);
  Serial.print(" - MILLIS: "); Serial.print(millis());

  Serial.println();
  delay(30);
}
void HT_goc(int hang)
{
  lcd.setCursor(2,hang); lcd.print("A:");
  lcd.setCursor(4,hang); lcd.print(servo_A.read());

  if(servo_B.read()<100)
  {
    lcd.setCursor(7,hang); lcd.print("B:");
    lcd.setCursor(9,hang); lcd.print(servo_B.read());
    lcd.setCursor(11,hang); lcd.print(" ");
  }
  else 
  {
    lcd.setCursor(7,hang); lcd.print("B:");
    lcd.setCursor(9,hang); lcd.print(servo_B.read());   
  }

  if(servo_Z.read() < 10)
  {
    lcd.setCursor(13,hang); lcd.print("Z:");
    lcd.setCursor(15,hang); lcd.print(servo_Z.read());
    lcd.setCursor(16,hang); lcd.print("  ");
  }
  else if(servo_Z.read() < 100)
  {
    lcd.setCursor(13,hang); lcd.print("Z:");
    lcd.setCursor(15,hang); lcd.print(servo_Z.read());
    lcd.setCursor(17,hang); lcd.print(" ");
  }  
  else 
  {
    lcd.setCursor(13,hang); lcd.print("Z:");
    lcd.setCursor(15,hang); lcd.print(servo_Z.read());
  }
}

void Xem_lenh()
{
  Serial.print("1="); Serial.print(EEPROM.read(1));
  Serial.print("  - CUOI="); Serial.println(EEPROM.read(2));
  for(int i=10; i<30; i++)
  {
    Serial.print(i); Serial.print("="); Serial.print(EEPROM.read(i)); Serial.print("; ");
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
}

void dk_servo()
{
  if(gt_nutnhan != macdinh)
  {
    if(gt_nutnhan == 0)
    {
      biendem += 1;
    }
    macdinh = gt_nutnhan;
  }
  if(biendem % 2 == 0) servo_kep.write(180); //Kep
  else  servo_kep.write(0); //Mo

  gt_bientro_A = analogRead(bientro_A); //Doc bien tro A
  goc_A = map(gt_bientro_A, 0, 1023, 50, 80); //chuyen doi sang goc Servo tu 0-180' | 50-80
  servo_A.write(goc_A); //Dieu khien Servo A

  gt_bientro_B = analogRead(bientro_B);
  goc_B = map(gt_bientro_B, 0, 1023, 50, 120); //50-120
  servo_B.write(goc_B);

  gt_bientro_Z = analogRead(bientro_Z);
  goc_Z = map(gt_bientro_Z, 0, 1023, 0, 180);    
  servo_Z.write(goc_Z);  
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

void man_hinh()
{
  lcd.setCursor(5,2);
  lcd.print("ARM ROBOT");
  lcd.setCursor(2,3);
  lcd.print("Green Technology");
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

void Them_lenh()
{
  if(KT_co_lenh() == true) //Da co lenh duoc luu
  {
    lcd.clear();
    lcd.setCursor(3,1);
    lcd.print("DA CO LENH...!"); 
    delay(5000);
    lcd.clear();
    man_hinh();
    dem_menu = 0; dem_lenxuong = 0;         
  }
  else //Chua co lenh nao duoc luu
  {
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("XIN MOI NHAP LENH");
    lcd.setCursor(4,1);
    lcd.print("VI TRI: ");
    lcd.setCursor(12,1);
    lcd.print(vitri);          
  }  
}

void luu_lenh()
{ 
  //6,7,8,9 ; 10,11,12,13 ; 14,15,16,17 ; 18,19,20,21
  diachi_A += 4;
  diachi_B += 4;
  diachi_Z += 4;
  diachi_Kep += 4;

  EEPROM.write(diachi_A, servo_A.read());
  EEPROM.write(diachi_B, servo_B.read());
  EEPROM.write(diachi_Z, servo_Z.read());
  EEPROM.write(diachi_Kep, servo_kep.read());
  delay(50);
}

void chay_lenh()
{
  if(KT_co_lenh() == true) //Neu co lenh
  {
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("CHUONG TRINH"); 
    lcd.setCursor(6,1);
    lcd.print("DANG CHAY");      

    byte doc_A; byte doc_B; byte doc_Z; byte doc_kep;
    ketthuc = EEPROM.read(2); //doc o nho cuoi cung
    while(run == true)
    {
      time = millis();
      gt_enter = digitalRead(enter);
      if(gt_enter == 1 && dem_menu==2 && dem_lenxuong == 3)
      {
        while(batdau < ketthuc)
        {
          gt_enter = digitalRead(enter);
          if(millis() - time > 500 && gt_enter == 1)
          {
            HT_goc(2);
            doc_A = EEPROM.read(batdau);
            doc_B = EEPROM.read(batdau + 1);
            doc_Z = EEPROM.read(batdau + 2);
            doc_kep = EEPROM.read(batdau + 3);

            servo_A.write(doc_A);
            servo_B.write(doc_B);
            servo_Z.write(doc_Z);
            servo_kep.write(doc_kep);

            Serial.print("Dem_menu: "); Serial.print(dem_menu); Serial.print(" | GT_ENTER: "); Serial.print(gt_enter);
            Serial.print("  - Time: "); Serial.print(time); Serial.print(" | Bat dau: "); Serial.print(batdau);
            Serial.print("  - A: "); Serial.print(doc_A); Serial.print("  - B: "); 
            Serial.print(doc_B); Serial.print("  - Z: "); Serial.print(doc_Z);
            Serial.print("  - KEP: "); Serial.println(doc_kep);

            time = millis();
            batdau += 4;              
          }
          else if(gt_enter == 0)
          {
            run = false;
            break;
          }  
        }
      }
      else if(gt_enter == 0 && dem_menu==2 && dem_lenxuong == 3)
      {
        run = false;        
        break;
      }

      batdau = 10;
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
