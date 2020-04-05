#define IS_SSID      1
#define IS_PASSWD    2
#define IS_APSSID    3
#define IS_APPASSWD  4
#define IS_ADMIN_U   5
#define IS_ADMIN_P   6

#define ON  0
#define OFF 1

#define SERIAL_BAUDRATE 9600

char *filenames[] = {"/alexaone.txt","/alexatwo.txt","/alexathree.txt","/alexafour.txt","/alexafive.txt","/alexasix.txt","/alexaseven.txt"};

char *FORM_INPUTS[] = {"input_command_one","input_command_two","input_command_three","input_command_four","input_command_five","input_command_six","input_command_seven"};

//teste
uint8_t newMACAddress[] = {0x32, 0xAE, 0xA4, 0x07, 0x0D, 0x66};

#define PIN_SDA      0
#define PIN_SCL      2
#define PCF_ADDR     0x27

char WIFI_SSID[50]         = {0};
char WIFI_PASS[50]         = {0};
char AP_WIFI_SSID[50]      = {0};
char AP_WIFI_PASS[50]      = {0};
char LOGIN_USER[50]        = {0};
char LOGIN_PASS[50]        = {0};

unsigned long int  HTTP_PORT = 80;

IPAddress staticIP(10, 0, 0, 1); //ESP static ip
IPAddress gateway(0, 0, 0, 0);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 0, 0, 0);  //Subnet mask

uint8_t bits_value = 0xFF;


char ALEXA_COMMANDS[7][50] = {{0}};

/* 
struct rly {
  char ALEXA_COMMAND_ONE[50]   = {0};
  char ALEXA_COMMAND_TWO[50]   = {0};
  char ALEXA_COMMAND_THREE[50] = {0};
  char ALEXA_COMMAND_FOUR[50]  = {0};
  char ALEXA_COMMAND_FIVE[50]  = {0};
  char ALEXA_COMMAND_SIX[50]   = {0};
  char ALEXA_COMMAND_SEVEN[50] = {0};
} caption;
*/

void startCredentials(){

  strcpy(WIFI_SSID,"laboratorio");
  strcpy(WIFI_PASS,"laboratorio123");

  char undefined[18] = {0};
  for (uint8_t i=0;i<7;i++){
    sprintf(undefined, "indefinido %d",i);
    strcpy(ALEXA_COMMANDS[i],undefined);
    memset(undefined,0,18);
  }
  
  strcpy(AP_WIFI_SSID,"AFInteliHome");
  strcpy(AP_WIFI_PASS,"afintelihome");

  strcpy(LOGIN_USER,"admin");
  strcpy(LOGIN_PASS,"admin");
}
