#include "mcc_generated_files/mcc.h"

#include "waveform_luts.h"

#define FUNC_SINE 0
#define FUNC_SAW 1
#define FUNC_SAW_REVERSE 2
#define FUNC_TRIANGLE 3
#define FUNC_SQUARE 4
#define FUNC_DOOM 5
#define FUNC_RNG 6
#define FUNCTION_COUNT 7

volatile uint8_t index = 0;
volatile uint16_t cycleCount = 0;
volatile uint8_t mode = FUNC_RNG;
uint16_t cv;

void main(void)
{
    INTCON0bits.GIE = 0;
    SYSTEM_Initialize();
    ADPCH = channel_ANA5;
    INTCON0bits.GIE = 1;
        
    while (1)
    { 
        // LUT Logic -----------------------------------------------------------  
        if(mode == FUNC_SINE){ AdcTargetValue[0] = SineLUT[index]; }
        if(mode == FUNC_SAW){ AdcTargetValue[0] = SawtoothLUT[index]; }
        if(mode == FUNC_SAW_REVERSE){ AdcTargetValue[0] = SawtoothLUT[255 - index]; }
        if(mode == FUNC_TRIANGLE){ AdcTargetValue[0] = TriangleLUT[index]; }
        if(mode == FUNC_SQUARE){ if(index & 128) {AdcTargetValue[0] = 255;} else {AdcTargetValue[0] = 0;} }
        if(mode == FUNC_DOOM){ AdcTargetValue[0] = DoomLUT[index]; }
        if(mode == FUNC_RNG){ AdcTargetValue[0] = DoomLUT[index ^ (cycleCount >> 8)]; }
        index += 1;
        
        // CV Conversion -------------------------------------------------------
        cv = ((ControlVoltage[1] << 8) + ControlVoltage[0]);
        
        // Frequency Control ---------------------------------------------------
        //To Do: Define CV to frequency function.
        DELAY_microseconds(cv >> 4);
        
        // Button Logic --------------------------------------------------------
        if(PORTCbits.RC0 == 0)
        {
            DELAY_milliseconds(1);
            while(PORTCbits.RC0 == 0) 
            { 
                LATCbits.LATC1 = 0;
                DELAY_milliseconds(1); 
            }            
            mode++;
            if(mode >= FUNCTION_COUNT)
            {
                mode = 0;
            }
        }
        LATCbits.LATC1 = 1;
        
        
        INTCON0bits.GIE = 1;
        
        cycleCount++;
    }
}
