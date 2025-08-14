#include "gase.h"

// 检测烟雾、可燃性气体
// 范围：0——100
// 阈值为30左右
u8 GASE_GET_EX()
{
  u8 temp_ex = (float)Get_Adc(GASE_EX) / 4095 * 100;
  return temp_ex;
}
 

// 检测酒精
// 范围：0——100
// 阈值为45左右
u8 GASE_GET_ET()
{
  u8 temp_et = (float)Get_Adc(GASE_ET) / 4095 * 100;
  return temp_et;
} 
