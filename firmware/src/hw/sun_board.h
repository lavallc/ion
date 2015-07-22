//includes for sun board (rev 1.0)
#ifndef SUN_BOARD_H
#define SUN_BOARD_H

#ifdef BOARD_SUN_1_1

#define kSunBoardPinRay1 5
#define kSunBoardPinRay2 6
#define kSunBoardPinRay3 29
#define kSunBoardPinRay4 30

#define kSunBoardNumberOfRays 4
#define kSunBoardNumberOfLedsPerRay 10

#define kSunBoardPinMicrophone 1

#define kSunBoardPinLairdWhisperDisable 20

#define kSunBoardEQStrobe       10
#define kSunBoardEQDataPin      4
#define kSunBoardEQDataADC      ADC_CONFIG_PSEL_AnalogInput5
#define kSunBoardEQReset  		11

#define kSunBoardCapsnseIRQPin 7

#define kSunBoardFTDISerialTx 3
#define kSunBoardFTDISerialRx 2

#define kSunBoardDigPin1 13
#define kSunBoardDigPin2 14
#define kSunBoardDigPin3 15
#endif


#ifdef BOARD_SUN_1_3

#define kSunBoardPinRay1 1
#define kSunBoardPinRay2 2
#define kSunBoardPinRay3 29
#define kSunBoardPinRay4 30

#define kSunBoardNumberOfRays 4
#define kSunBoardNumberOfLedsPerRay 10

#define kSunBoardPinMicrophone 5

#define kSunBoardPinLairdWhisperDisable 20

#define kSunBoardEQStrobe       5
#define kSunBoardEQDataPin      4
#define kSunBoardEQDataADC      ADC_CONFIG_PSEL_AnalogInput5
#define kSunBoardEQReset  		6

#define kSunBoardCapsnseIRQPin 7

#define kSunBoardFTDISerialTx 22
#define kSunBoardFTDISerialRx 23

#define kSunBoardDigPin1 10
#define kSunBoardDigPin2 14
#define kSunBoardDigPin3 18

#endif


#ifdef BOARD_SUN_1_3B

#define kSunBoardPinRay1 30
#define kSunBoardPinRay2 0
#define kSunBoardPinRay3 25
#define kSunBoardPinRay4 24

#define kSunBoardNumberOfRays 4
#define kSunBoardNumberOfLedsPerRay 10

#define kSunBoardPinMicrophone 1

#define kSunBoardPinLairdWhisperDisable 20

#define kSunBoardEQStrobe       3
#define kSunBoardEQDataPin      2
#define kSunBoardEQDataADC      ADC_CONFIG_PSEL_AnalogInput3
#define kSunBoardEQReset  		4

#define kSunBoardCapsnseIRQPin 5

#define kSunBoardFTDISerialTx 22
#define kSunBoardFTDISerialRx 21

#define kSunBoardDigPin1 8
#define kSunBoardDigPin2 9
#define kSunBoardDigPin3 10

#endif


#endif
