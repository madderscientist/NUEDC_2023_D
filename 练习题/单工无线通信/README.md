# 单工无线通信
"发射端_ZET6"和"接收端_C8T6"是第一版。模拟信号还好，数字信号用usart，收不到。<br>
解决办法是改更低的波特率，和用更大的幅值差异。体现在了"发射端ASK"和"接收端300波特率"，前者用AD9954发射ASK信号(模拟usart协议)，后者改了波特率，具体操作是：在串口初始化启用时钟前用
```C
RCC_PCLK2Config(RCC_HCLK_Div4);
```
把usart1的频率改为18M，就可以用300波特率了。<br>

        附：关于stm32主频和波特率的关系：
        https://blog.csdn.net/FLYMCUCN/article/details/129042583 指出了波特率受主频限制
        https://blog.csdn.net/qq_15181569/article/details/115332981 指出了波特率的计算
        https://blog.51cto.com/u_15127670/4095785 指出了如何修改主频

        改主频可以直接修改system_stm32f10x.c的SetSysClockTo72函数。对比不同的主频配置函数，发现只有两处不同：
        1. 1018行的FLASH_ACR_LATENCY_<i>。https://blog.csdn.net/xiaoqa11/article/details/119281886 指出了不同主频对应的i值
        2. 1056行的RCC_CFGR_PLLMULL<i>。指倍频系数，主频=8M*这个系数。
        两个参数自行在stm32f10x.h中查找可替换的值

发射端代码主要写了一个LCD屏幕的按钮UI系统，在UI.c和UI.h中。