#ifndef PATTERNS_H
#define PATTERNS_H

static const char* cppPattern =
        R"cpp(
/*********************************************************
 * ISR%1.cpp
 * Automatic generated source file for MCU %1
 *
 *
 **/
#include "ISR%1.h"
__attribute__((naked, noreturn)) void ISR::DefaultHandler()
{
   for(;;);
}


#ifdef USE_MEMORY_ISR
__attribute__((section(".isr_vector"))) const ISR::ShortVectors interruptsVectorTable =
#else
__attribute__((section(".isr_vector"))) const ISR::Vectors interruptsVectorTable =
#endif
{
#ifdef USE_MEMORY_ISR
   _estack,
   ISR::Reset
};

__attribute__((section(".isr_vector2"))) ISR::Vectors MeminterruptsVectorTable =
{
#endif
   _estack,
%2
};
%3
)cpp";


static const char* hPattern =
        R"hhh(
/*********************************************************
 * ISR%1.h
 * Automatic generated header file for MCU %1
 *
 *
 **/
#ifndef INTERRUPTS_H
#define INTERRUPTS_H
#include <stdint.h>
extern uint32_t _estack;
//#define USE_MEMORY_ISR

extern "C"
{
    namespace ISR
    {
        void DefaultHandler();
        struct ShortVectors
        {
            uint32_t estack;
            void (*Reset )(void);
        };

        struct Vectors
        {
            uint32_t estack;
%2
        };
%3
    }

}


#endif // INTERRUPTS_H
)hhh";


#endif // PATTERNS_CPP
