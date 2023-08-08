#include "sys.h"
#include "lcd.h"
#include "UI.h"
#include "stdio.h"
#include "delay.h"
#include "stimuSerial.h"

#define W 240
#define H 320
#define btn_size 34   // ����һ��7������Ļ��Ϊ240����
/*
tp_dev.touchtype!=0XFF -> ���ǵ�����Ļ
������ɫ��POINT_COLOR=RED;   ��ɫ��16λ��ɫ��[15:11]red[10:5]green[4:0]blue
�жϰ��£�tp_dev.sta&TP_PRES_DOWN==1
��ȡ���µ�λ�ã�tp_dev.x[0]��tp_dev.y[0]
���ڻ�ͼ������(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)
������Ϣ��lcddev.width��lcddev.height��240*320��
д�֣�LCD_ShowString(lcddev.width-24,0,200,16,16,"RST");20���У��ƺ�ֻ��16����
*/
u16 id = 0;

u8 key_size = 28;
typedef struct{
    u16 x;
    u16 y;
    char c;
} btn;
btn guide[7] = {   // ʹ��asciiǰ��Ĳ��ɼ��ַ�����Ϊ���⹦�� yû�м�һ���������
    {0*btn_size, H-btn_size, 1},    // �����л�
    {1*btn_size, H-btn_size, 2},    // ��Сд�л�
    {2*btn_size, H-btn_size, 3},    // �����л�
    {3*btn_size, H-btn_size, 4},    // ����
    {4*btn_size, H-btn_size, ' '},  // �ո�
    {5*btn_size, H-btn_size, 5},   // ɾ��
    {6*btn_size, H-btn_size, 6}    // ȷ��
};
btn nums[13] = {
    {2*btn_size, H-5*btn_size-1, '1'},
    {3*btn_size, H-5*btn_size-1, '2'},
    {4*btn_size, H-5*btn_size-1, '3'},
    {2*btn_size, H-4*btn_size-1, '4'},
    {3*btn_size, H-4*btn_size-1, '5'},
    {4*btn_size, H-4*btn_size-1, '6'},
    {2*btn_size, H-3*btn_size-1, '7'},
    {3*btn_size, H-3*btn_size-1, '8'},
    {4*btn_size, H-3*btn_size-1, '9'},
    {3*btn_size, H-2*btn_size-1, '0'},

    {2*btn_size, H-2*btn_size-1, 5},    // ����
    {4*btn_size, H-2*btn_size-1, 6},    // ����
    {5*btn_size, H-2*btn_size-1, 7}     // ����
};
btn signs[28] = {
    {0*btn_size, H-5*btn_size-1, '!'},
    {1*btn_size, H-5*btn_size-1, '?'},
    {2*btn_size, H-5*btn_size-1, ';'},
    {3*btn_size, H-5*btn_size-1, ':'},
    {4*btn_size, H-5*btn_size-1, '+'},
    {5*btn_size, H-5*btn_size-1, '-'},
    {6*btn_size, H-5*btn_size-1, '='},//
    {0*btn_size, H-4*btn_size-1, '('},
    {1*btn_size, H-4*btn_size-1, ')'},
    {2*btn_size, H-4*btn_size-1, '['},
    {3*btn_size, H-4*btn_size-1, ']'},
    {4*btn_size, H-4*btn_size-1, '{'},
    {5*btn_size, H-4*btn_size-1, '}'},
    {6*btn_size, H-4*btn_size-1, '"'},//
    {0*btn_size, H-3*btn_size-1, '<'},
    {1*btn_size, H-3*btn_size-1, '>'},
    {2*btn_size, H-3*btn_size-1, '@'},
    {3*btn_size, H-3*btn_size-1, '#'},
    {4*btn_size, H-3*btn_size-1, '$'},
    {5*btn_size, H-3*btn_size-1, '%'},
    {6*btn_size, H-3*btn_size-1, '\''},//
    {0*btn_size, H-2*btn_size-1, '^'},
    {1*btn_size, H-2*btn_size-1, '&'},
    {2*btn_size, H-2*btn_size-1, '*'},
    {3*btn_size, H-2*btn_size-1, '\\'},
    {4*btn_size, H-2*btn_size-1, '/'},
    {5*btn_size, H-2*btn_size-1, ','},
    {6*btn_size, H-2*btn_size-1, '.'},//
};
btn alphabet[28] = {
    {0*btn_size, H-5*btn_size-1, 'a'},
    {1*btn_size, H-5*btn_size-1, 'b'},
    {2*btn_size, H-5*btn_size-1, 'c'},
    {3*btn_size, H-5*btn_size-1, 'd'},
    {4*btn_size, H-5*btn_size-1, 'e'},
    {5*btn_size, H-5*btn_size-1, 'f'},
    {6*btn_size, H-5*btn_size-1, 'g'},//
    {0*btn_size, H-4*btn_size-1, 'h'},
    {1*btn_size, H-4*btn_size-1, 'i'},
    {2*btn_size, H-4*btn_size-1, 'j'},
    {3*btn_size, H-4*btn_size-1, 'k'},
    {4*btn_size, H-4*btn_size-1, 'l'},
    {5*btn_size, H-4*btn_size-1, 'm'},
    {6*btn_size, H-4*btn_size-1, 'n'},//
    {0*btn_size, H-3*btn_size-1, 'o'},
    {1*btn_size, H-3*btn_size-1, 'p'},
    {2*btn_size, H-3*btn_size-1, 'q'},
    {3*btn_size, H-3*btn_size-1, 'r'},
    {4*btn_size, H-3*btn_size-1, 's'},
    {5*btn_size, H-3*btn_size-1, 't'},
    {6*btn_size, H-3*btn_size-1, 'u'},//
    {0*btn_size, H-2*btn_size-1, 'v'},
    {1*btn_size, H-2*btn_size-1, 'w'},
    {2*btn_size, H-2*btn_size-1, 'x'},
    {3*btn_size, H-2*btn_size-1, 'y'},
    {4*btn_size, H-2*btn_size-1, 'z'},
    {5*btn_size, H-2*btn_size-1, ','},
    {6*btn_size, H-2*btn_size-1, '.'},//
};
btn* currentBoard = alphabet;

void drawKeyboard(){
    POINT_COLOR=BLACK;
    LCD_Fill(0, H-5*btn_size-1, W-1, H-btn_size-1, WHITE);
    for (u8 i = 0; i < key_size; i++) {
        LCD_DrawRectangle(currentBoard[i].x,currentBoard[i].y,currentBoard[i].x+btn_size,currentBoard[i].y+btn_size);
        LCD_ShowChar(currentBoard[i].x+btn_size/2-8,currentBoard[i].y+btn_size/2-8,currentBoard[i].c,16,1);
    }
}
void drawGuide(){
    POINT_COLOR=RED;
    const char* s[7]= {"123","Aa","#@&","back","spa","del","yes"};
    for (u8 i = 0; i < 7; i++) {
        LCD_ShowString(guide[i].x+5,guide[i].y+6,btn_size,btn_size,16,(u8*)s[i]);
        LCD_DrawRectangle(guide[i].x,guide[i].y,guide[i].x+btn_size,guide[i].y+btn_size);
    }
}

#define message_len 15*5
char message[message_len+1] = {0};
u8 mlen = 0;    // ��ǰmessage�ĳ���
///////////////////////////////////////////////////
// ����ģʽ�µ�ģʽ�л�
///////////////////////////////////////////////////
void SwitchAa(u8 mode){     // 1: Сд 0����д
    key_size = 28;
    currentBoard = alphabet;
    int8_t offset = mode - (alphabet[0].c>94);
    if(offset!=0){
        offset *= 32;
        for(u8 i=0;i<26;i++)
            alphabet[i].c+=offset;
    }
    drawKeyboard();
}
void Switch123(){
    key_size = 10;  // �����������ʾ
    currentBoard = nums;
    drawKeyboard();
}
void SwitchSign(){
    key_size = 28;
    currentBoard = signs;
    drawKeyboard();
}
///////////////////////////////////////////////////////
// �õ���ť
///////////////////////////////////////////////////////
char getKey1(u16 x, u16 y){
    for (u8 i = 0; i < key_size; i++) {
        if(x>currentBoard[i].x&&x<currentBoard[i].x+btn_size&&y>currentBoard[i].y&&y<currentBoard[i].y+btn_size)
            return currentBoard[i].c;
    }
    return 0;
}
char getKey2(u16 x, u16 y){     // ����ģʽ
    for (u8 i = 0; i < key_size; i++) {
        if(x>currentBoard[i].x&&x<currentBoard[i].x+btn_size&&y>currentBoard[i].y&&y<currentBoard[i].y+btn_size)
            return currentBoard[i].c;
    }
    for (u8 i = 0; i < 7; i++) {
        if(x>guide[i].x&&x<guide[i].x+btn_size&&y>guide[i].y&&y<guide[i].y+btn_size)
            return guide[i].c;
    }
    return 0;
}
char(*getKey)(u16, u16) = getKey1;


///////////////////////////////////////////////////////
// ���ڷ��͹���
// Э�飺��һλ���ж�λ��1��ʾ�ȶԵ�ַ��0��ʾ����
///////////////////////////////////////////////////////
void sendTxt(){
    u8 i = id|0x80;
    stimuSerial_SendByte(i);
    for(i=0;i<mlen;i++){
        stimuSerial_SendByte(message[i]);
    }
    stimuSerial_SendByte(127|0x80);  // ������ֹλ
}
void sendVoc(){
    stimuSerial_SendByte(id|0x80);
    stimuSerial_SendByte(126|0x80);  // ����������־
}
///////////////////////////////////////////////////////
// ��ͬģʽ�µİ�������
///////////////////////////////////////////////////////
void function1(char key){   // ѡ�����
    if(key<10) {
        switch(key){
            case 5:// ɾ��
                if(mlen>0){
                    mlen--;
                    LCD_ShowChar(16*(mlen%15), 16*(mlen/15), ' ', 16, 0);
                    id = id / 10;
                }
                break;
            case 6:// ����
                textUI();
                break;
            case 7:// ����
                sendVoc();
                LCD_ShowString(87,120,100,16,16,(u8*)"VoiceMode!");
                delay_ms(2000);
                callUI();
                break;
        }
    } else {
            id = id * 10 + key - '0';
            if(id>125){     // 126��127������;
                id = 125;
                message[0]='1';
                message[1]='2';
                message[2]='5';
                message[3]=0;
                for(u8 i = 0;i<3;i++){
                    LCD_ShowChar(16*(i%15), 16*(i/15), message[i], 16, 0);
                }
                mlen = 3;
            }else {
                LCD_ShowChar(16*(mlen%15), 16*(mlen/15), key, 16, 1);
                message[mlen] = key;
                mlen++;
            }
    }
}
void function2(char key){   // �༭����
    if(key<10) {
        switch(key){
            case 1:// �����л�
                Switch123();
                break;
            case 2:// ��Сд�л�
                SwitchAa(currentBoard[0].c<97);
                break;
            case 3:// �����л�
                SwitchSign();
                break;
            case 4:// ����(���Ž���)
                callUI();
                break;
            case 5:// ɾ��
                if(mlen>0){
                    mlen--;
                    LCD_ShowChar(16*(mlen%15), 16*(mlen/15), ' ', 16, 0);
                }
                break;
            case 6:// ȷ��(���Ͷ���)
                LCD_ShowString(87,120,100,16,16,(u8*)"Sending..");
                sendTxt();
                LCD_ShowString(87,120,100,16,16,(u8*)"Succeed!!");
                delay_ms(1000);
                callUI();
                break;
        }
    } else {
        if(mlen < message_len) {
            message[mlen] = key;
            LCD_ShowChar(16*(mlen%15), 16*(mlen/15), key, 16, 1);
            mlen++;
        }
    }
}
void(*function)(char) = function1;
///////////////////////////////////////////////////////
// ģʽ�л��ͳ�ʼ��
///////////////////////////////////////////////////////
void callUI(){
    id = 0;
    LCD_Clear(WHITE);
    key_size = 10;
    currentBoard = nums;
    drawKeyboard();
    function = function1;
    getKey = getKey1;
    mlen = 0;
    // ���϶�İ�ť
    const char* s[3]= {"del","txt","voc"};
    for (u8 i = 10; i < 13; i++) {
        LCD_ShowString(currentBoard[i].x+5,currentBoard[i].y+6,btn_size,btn_size,16,(u8*)s[i-10]);
        LCD_DrawRectangle(currentBoard[i].x,currentBoard[i].y,currentBoard[i].x+btn_size,currentBoard[i].y+btn_size);
    }
    key_size = 13;
}
void textUI(){
    LCD_Clear(WHITE);
    drawGuide();
    SwitchAa(1);
    function = function2;
    getKey = getKey2;
    mlen = 0;
    // ��ʾĿ��ID
    char msg[10] = {0};
    sprintf(msg,"to id %d",id);
    LCD_ShowString(87,120,100,16,16,(u8*)msg);
}
