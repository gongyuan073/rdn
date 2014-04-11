#ifndef __CONTROL_H__
#define __CONTROL_H__

void W_Measure(unsigned char num);
void Adherent_Detect(void);
void Adjust_T(float Measure_W,float Set_W);
void Adjust_P(unsigned int Measure,unsigned int Set);
void Temperature_Control(void);
void Power_Control(void);
void LCD_Flush(unsigned char electrode_num);

#endif
