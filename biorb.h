#define BIORB_DELAY_8HR      0x807FB946
#define BIORB_DELAY_10HR     0x807F39C6
#define BIORB_DELAY_12HR     0x807F7986
#define BIORB_DELAY_14HR     0x807FF906

#define BIORB_ON              0x807FA15E
#define BIORB_OFF             0x807F21DE
#define BIORB_RESET           0x807F619E
#define BIORB_MODULATE        0x807FE11E

#define BIORB_RED             0x807F916E
#define BIORB_GREEN           0x807F11EE
#define BIORB_BLUE            0x807F51AE
#define BIORB_WHITE           0x807FD12E

#define BIORB_BROWN           0x807FB14E
#define BIORB_WOOD            0x807F31CE
#define BIORB_SKY             0x807F718E
#define BIORB_BRIGHTNESS_100  0x807FF10E

#define BIORB_PINK            0x807FA956
#define BIORB_MEADOWGREEN     0x807F29D6
#define BIORB_DEEPVIOLETT     0x807F6996
#define BIORB_BRIGHTNESS_80   0x807FE916

#define BIORB_ORANGE          0x807F9966
#define BIORB_LIGHTGREEN      0x807F19E6
#define BIORB_VIOLETT         0x807F59A6
#define BIORB_BRIGHTNESS_60   0x807FD926

#define BIORB_YELLOW          0x807F8976
#define BIORB_PETROL          0x807F09F6
#define BIORB_PURPLE          0x807F49B6
#define BIORB_BRIGHTNESS_40   0x807FC936

#define IR_PIN    D4     


void SendCode(uint32_t code);
//void SendCode(String code);
void SetColor ( uint32_t colorCode, uint32_t BrightnessCode, bool silent=0 );

