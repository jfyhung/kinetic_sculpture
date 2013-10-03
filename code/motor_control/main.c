//============================================================================
// 乙班_第八組   陳易駿 洪浩哲
// 動態球藝術
//
// 功能:
// 1.將控制旋轉方向的code放到main中
// 2.KEY1~KEY5分別有不同的動作(KEY5為測試是否能夠正常運作使用)
// 3.順時針轉讓球上升，逆時針轉讓球下降
//============================================================================

#include "io430.h"
#include "in430.h"
#include "intrinsics.h"

#define DELAY_2000MS 0x2000
#define DELAY_1000MS 0x1000

int KEY_NUM; //a variable that indicates the pushed button

void stop_motor();
void action_1();
void action_2();
void action_3();
void action_4();
void action_5();

void main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;

  //============================================================================
  // port2 settings(buttons)
  //============================================================================
  P2IE |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT4);               // P2.0, P2.3 and P2.4 interrupt enabled
  P2IES |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT4);               // P2.0, P2.3 and P2.4  Hi/Lo edge
  P2IFG = 0;               // P2.0, P2.3 and P2.4  IFG cleared

  //============================================================================
  // port1 settings(output pins)
  //============================================================================
  P1DIR |= (BIT2 + BIT3 + BIT4 + BIT5);               // P1.2 to P1.5 output
  P1SEL |= (BIT2 + BIT3 + BIT4 + BIT5);               // P1.2 to P1.5 options select

  //============================================================================
  // TA0 settings
  //
  // use PWM modulation, each channel for one motor.
  // initially set all motors to stop state.(TA0CCRx = 0)
  //============================================================================
  TA0CTL = TASSEL_1 | MC_1 | ID_0 | TACLR;              // ACLK, up mode, clear TAR
  TA0CCR0 = 655;               // PWM Period(20ms)
  TA0CCTL1 = OUTMOD_7;         // CCR1 reset/set
  TA0CCTL2 = OUTMOD_7;         // CCR1 reset/set
  TA0CCTL3 = OUTMOD_7;         // CCR1 reset/set
  TA0CCTL4 = OUTMOD_7;         // CCR1 reset/set
  TA0CCR1 = 0;                 // CCR1 PWM duty cycle(1ms)  freeze!
  TA0CCR2 = 0;
  TA0CCR3 = 0;
  TA0CCR4 = 0;

  //============================================================================
  // TA1 settings
  //
  // another timer that used as a delay timer
  //============================================================================
  TA1CTL = TASSEL_1 | ID_3 | MC_0; // ACLK/8, stop mode, no need to clear
  //TA1CCR1 = TA1R + DELAY_2000MS; // "Compare" value for 2000ms delay


  while(1)
  {
      __low_power_mode_4();    //set MSP430 into low power mode 4, react only button interrupts

      switch(KEY_NUM)
      {
        case  0:            // Vector  0:  No interrupt
          break;
        case  2:            // Vector  2:  P2IFG0
          action_1();
          break;
        case  4:            // Vector  4:  P2IFG1
          action_2();
          break;
        case  6:            // Vector  6:  P2IFG2
          action_3();
          break;
        case  8:            // Vector  8:  P2IFG3
          action_4();
          break;
        case 10:            // Vector 10:  P2IFG4
          action_5();
          break;
        case 12:            // Vector 10:  P2IFG5
          break;
        case 14:            // Vector 10:  P2IFG6
          break;
        case 16:            // Vector 10:  P2IFG7
          break;
        default:
          break;
    }
    stop_motor();                 // stop motor
    TA1CTL &= ~MC_2;              // stop timer
    TA1CCTL1_bit.CCIE = 0;        // timer interrupt disabled
    P2IE |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT4); // PORT2 interrupt enabled
  }

}

// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
  switch(__even_in_range(P2IV,16))
  {
  case  0:           // Vector  0:  No interrupt
    break;
  case  2:           // Vector  2:  P2IFG0
    KEY_NUM = 2;
    __low_power_mode_off_on_exit();
    break;
  case  4:           // Vector  4:  P2IFG1
    KEY_NUM = 4;
    __low_power_mode_off_on_exit();
    break;
  case  6:           // Vector  6:  P2IFG2
    KEY_NUM = 6;
    __low_power_mode_off_on_exit();
    break;
  case  8:           // Vector  8:  P2IFG3
    KEY_NUM = 8;
    __low_power_mode_off_on_exit();
    break;
  case 10:           // Vector 10:  P2IFG4
    KEY_NUM = 10;
    __low_power_mode_off_on_exit();
    break;
  case 12:           // Vector 10:  P2IFG5
    break;
  case 14:           // Vector 10:  P2IFG6
    break;
  case 16:           // Vector 10:  P2IFG7
    break;
  default:
    break;
  }
}

// TA1 interrupt sevice routine
#pragma vector = TIMER1_A1_VECTOR
__interrupt void TIMERA1_ISR (void)
{
  TA1CCTL1_bit.CCIFG = 0;
  __low_power_mode_off_on_exit();
}

void stop_motor()// stop all 4 motors
{
  TA0CCR1 = 0;
  TA0CCR2 = 0;
  TA0CCR3 = 0;
  TA0CCR4 = 0;
}

void action_1()
{
  P2IE = 0;                     // PORT2 interrupt disabled
  TA0CCR1 = 33;                 // the motor will rotate clockwise
  TA0CCR2 = 0;                  // the motor will stop
  TA0CCR3 = 0;                  // the motor will stop
  TA0CCR4 = 0;                  // the motor will stop
  P2IFG &= ~BIT4;               // clean P2IFG4
  TA1CCTL1_bit.CCIE = 1;        // timer interrupt enabled
  TA1CCR1 = TA1R + DELAY_2000MS;// set 1000ms delay
  TA1CTL |= MC_2;               // start timer
  __low_power_mode_3();         // enter low power mode
  //TA1CTL |= MC_0;               // stop timer
  TA0CCR1 = 66;                 // the motor will rotate counterclockwise
  TA0CCR2 = 33;                 // the motor will rotate clockwise
  TA0CCR3 = 0;                  // the motor will stop
  TA0CCR4 = 0;                  // the motor will stop
  TA1CCR1 += DELAY_2000MS;      // set 2000ms delay
  //TA1CTL |= MC_2;             // start timer
  __low_power_mode_3();         // enter low power mode
  TA0CCR1 = 0;                  // the motor will stop
  TA0CCR2 = 66;                 // the motor will rotate counterclockwise
  TA0CCR3 = 33;                 // the motor will rotate clockwise
  TA0CCR4 = 0;                  // the motor will stop
  TA1CCR1 += DELAY_2000MS;      // set 2000ms delay
  __low_power_mode_3();         // enter low power mode
  TA0CCR1 = 0;                  // the motor will stop
  TA0CCR2 = 0;                  // the motor will stop
  TA0CCR3 = 66;                 // the motor will rotate counterclockwise
  TA0CCR4 = 33;                 // the motor will rotate clockwise
  TA1CCR1 += DELAY_2000MS;      // set 2000ms delay
  __low_power_mode_3();         // enter low power mode
  TA0CCR1 = 0;                  // the motor will stop
  TA0CCR2 = 0;                  // the motor will stop
  TA0CCR3 = 0;                  // the motor will stop
  TA0CCR4 = 66;                 // the motor will rotate counterclockwise
  TA1CCR1 += DELAY_2000MS;      // set 2000ms delay
  __low_power_mode_3();         // enter low power mode
}

void action_2()
{
  P2IE = 0;                     // PORT2 interrupt disabled
  TA0CCR1 = 33;                 // the motor will rotate clockwise
  TA0CCR2 = 0;                  // the motor will stop
  TA0CCR3 = 66;                 // the motor will rotate counterclockwise
  TA0CCR4 = 0;                  // the motor will stop
  P2IFG &= ~BIT4;               // clean P2IFG4
  TA1CCTL1_bit.CCIE = 1;        // timer interrupt enabled
  TA1CCR1 = TA1R + DELAY_2000MS;// set 2000ms delay
  TA1CTL |= MC_2;               // start timer
  __low_power_mode_3();         // enter low power mode
  //TA1CTL |= MC_0;             // stop timer
  TA0CCR1 = 0;                  // the motor stop
  TA0CCR2 = 33;                 // the motor will rotate clockwise
  TA0CCR3 = 0;                  // the motor stop
  TA0CCR4 = 66;                 // the motor will rotate counterclockwise
  TA1CCR1 += DELAY_2000MS;      // set 2000ms delay
  //TA1CTL |= MC_2;             // start timer
  __low_power_mode_3();         // enter low power mode
  TA0CCR1 = 66;                 // the motor will rotate counterclockwise
  TA0CCR2 = 66;                 // the motor will rotate counterclockwise
  TA0CCR3 = 33;                 // the motor will rotate clockwise
  TA0CCR4 = 33;                 // the motor will rotate clockwise
  TA1CCR1 += DELAY_2000MS;      // set 2000ms delay
  __low_power_mode_3();         // enter low power mode
  TA0CCR1 = 66;                 // the motor will rotate counterclockwise
  TA0CCR2 = 66;                 // the motor will rotate counterclockwise
  TA0CCR3 = 33;                 // the motor will rotate clockwise
  TA0CCR4 = 33;                 // the motor will rotate clockwise
  TA1CCR1 += DELAY_2000MS;      // set 2000ms delay
  __low_power_mode_3();         // enter low power mode
  TA0CCR1 = 33;                 // the motor will rotate clockwise
  TA0CCR2 = 33;                 // the motor will rotate clockwise
  TA0CCR3 = 66;                 // the motor will rotate counterclockwise
  TA0CCR4 = 66;                 // the motor will rotate counterclockwise
  TA1CCR1 += DELAY_2000MS;      // set 2000ms delay
  __low_power_mode_3();         // enter low power mode
}

void action_3()
{
  P2IE = 0;                     // PORT2 interrupt disabled
  TA0CCR1 = 33;                 // the motor will rotate clockwise
  TA0CCR2 = 33;                 // the motor will rotate clockwise
  TA0CCR3 = 33;                 // the motor will rotate clockwise
  TA0CCR4 = 33;                 // the motor will rotate clockwise
  P2IFG &= ~BIT4;               // clean P2IFG4
  TA1CCTL1_bit.CCIE = 1;        // timer interrupt enabled
  TA1CCR1 = TA1R + DELAY_2000MS;// set 2000ms delay
  TA1CTL |= MC_2;               // start timer
  __low_power_mode_3();         // enter low power mode
  //TA1CTL |= MC_0;             // stop timer
  TA0CCR1 = 66;                 // the motor will rotate counterclockwise
  TA0CCR2 = 0;                  // the motor will stop
  TA0CCR3 = 0;                  // the motor will stop
  TA0CCR4 = 0;                  // the motor will stop
  TA1CCR1 += DELAY_1000MS;      // set 1000ms delay
  //TA1CTL |= MC_2;             // start timer
  __low_power_mode_3();         // enter low power mode
  TA0CCR1 = 66;                 // the motor will rotate counterclockwise
  TA0CCR2 = 66;                 // the motor will rotate counterclockwise
  TA0CCR3 = 0;                  // the motor will stop
  TA0CCR4 = 0;                  // the motor will stop
  TA1CCR1 += DELAY_1000MS;      // set 1000ms delay
  __low_power_mode_3();         // enter low power mode
  TA0CCR1 = 0;                  // the motor will stop
  TA0CCR2 = 66;                 // the motor will rotate counterclockwise
  TA0CCR3 = 0;                  // the motor will stop
  TA0CCR4 = 66;                 // the motor will rotate counterclockwise
  TA1CCR1 += DELAY_1000MS;      // set 1000ms delay
  __low_power_mode_3();         // enter low power mode
  TA0CCR1 = 0;                  // the motor will stop
  TA0CCR2 = 0;                  // the motor will stop
  TA0CCR3 = 66;                 // the motor will rotate counterclockwise
  TA0CCR4 = 66;                 // the motor will rotate counterclockwise
  TA1CCR1 += DELAY_1000MS;      // set 1000ms delay
  __low_power_mode_3();         // enter low power mode
  TA0CCR1 = 0;                  // the motor will stop
  TA0CCR2 = 0;                  // the motor will stop
  TA0CCR3 = 66;                 // the motor will rotate counterclockwise
  TA0CCR4 = 0;                  // the motor will stop
  TA1CCR1 += DELAY_1000MS;      // set 1000ms delay
  __low_power_mode_3();         // enter low power mode
}

void action_4()
{
  P2IE = 0;                     // PORT2 interrupt disabled
  TA0CCR1 = 0;                  // the motor will stop
  TA0CCR2 = 0;                  // the motor will stop
  TA0CCR3 = 66;                 // the motor will rotate counterclockwise
  TA0CCR4 = 0;                  // the motor will stop
  P2IFG &= ~BIT4;               // clean P2IFG4
  TA1CCTL1_bit.CCIE = 1;        // timer interrupt enabled
  TA1CCR1 = TA1R + DELAY_2000MS;// set 2000ms delay
  TA1CTL |= MC_2;               // start timer
  __low_power_mode_3();         // enter low power mode
  //TA1CTL |= MC_0;             // stop timer
  TA0CCR1 = 66;                 // the motor will rotate counterclockwise
  TA0CCR2 = 33;                 // the motor will rotate clockwise
  TA0CCR3 = 0;                  // the motor will stop
  TA0CCR4 = 33;                 // the motor will rotate clockwise
  TA1CCR1 += DELAY_1000MS;      // set 1000ms delay
  //TA1CTL |= MC_2;             // start timer
  __low_power_mode_3();         // enter low power mode
  TA0CCR1 = 0;                  // the motor will stop
  TA0CCR2 = 33;                 // the motor will rotate clockwise
  TA0CCR3 = 33;                 // the motor will rotate clockwise
  TA0CCR4 = 33;                 // the motor will rotate clockwise
  TA1CCR1 += DELAY_1000MS;      // set 1000ms delay
  __low_power_mode_3();         // enter low power mode
  TA0CCR1 = 66;                 // the motor will rotate counterclockwise
  TA0CCR2 = 0;                  // the motor will stop
  TA0CCR3 = 66;                 // the motor will rotate counterclockwise
  TA0CCR4 = 0;                  // the motor will stop
  TA1CCR1 += DELAY_1000MS;      // set 1000ms delay
  __low_power_mode_3();         // enter low power mode
  stop_motor();                 // all the motor will stop
  TA1CCR1 += DELAY_1000MS;      // set 1000ms delay
  __low_power_mode_3();         // enter low power mode
  TA0CCR1 = 33;                 // the motor will rotate clockwise
  TA0CCR2 = 66;                 // the motor will rotate counterclockwise
  TA0CCR3 = 33;                 // the motor will rotate clockwise
  TA0CCR4 = 66;                 // the motor will rotate counterclockwise
  TA1CCR1 += DELAY_2000MS;      // set 2000ms delay
  __low_power_mode_3();         // enter low power mode
}

void action_5()
{
  P2IE = 0;                     // PORT2 interrupt disabled
  TA0CCR1 = 66;                 // the motor will rotate counterclockwise
  TA0CCR2 = 66;                 // the motor will rotate counterclockwise
  TA0CCR3 = 66;                 // the motor will rotate counterclockwise
  TA0CCR4 = 66;                 // the motor will rotate counterclockwise
  P2IFG &= ~BIT4;               // clean P2IFG4
  TA1CCTL1_bit.CCIE = 1;        // timer interrupt enabled
  TA1CCR1 = TA1R + DELAY_2000MS;// set 2000ms delay
  TA1CTL |= MC_2;               // start timer
  __low_power_mode_3();         // enter low power mode
  //TA1CTL |= MC_0;             // stop timer
  TA0CCR1 = 33;                 // the motor will rotate clockwise
  TA0CCR2 = 33;                 // the motor will rotate clockwise
  TA0CCR3 = 33;                 // the motor will rotate clockwise
  TA0CCR4 = 33;                 // the motor will rotate clockwise
  TA1CCR1 += DELAY_2000MS;      // set 2000ms delay
  //TA1CTL |= MC_2;             // start timer
  __low_power_mode_3();         // enter low power mode
  //TA1CTL |= MC_0;             // stop timer
  //stop_motor();               // stop motor
  //P2IE |= BIT4;               // P2.4 interrupt enabled
}
