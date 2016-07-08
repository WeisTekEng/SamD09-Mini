/* ---------------------------------------------------------------------------- */
/*                  Atmel Microcontroller Software Support                      */
/*                       SAM Software Package License                           */
/* ---------------------------------------------------------------------------- */
/* Copyright (c) 2014, Atmel Corporation                                        */
/*                                                                              */
/* All rights reserved.                                                         */
/*                                                                              */
/* Redistribution and use in source and binary forms, with or without           */
/* modification, are permitted provided that the following condition is met:    */
/*                                                                              */
/* - Redistributions of source code must retain the above copyright notice,     */
/* this list of conditions and the disclaimer below.                            */
/*                                                                              */
/* Atmel's name may not be used to endorse or promote products derived from     */
/* this software without specific prior written permission.                     */
/*                                                                              */
/* DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR   */
/* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE   */
/* DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,      */
/* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT */
/* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,  */
/* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    */
/* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING         */
/* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, */
/* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                           */
/* ---------------------------------------------------------------------------- */

#ifndef _SAM4N_UART3_INSTANCE_
#define _SAM4N_UART3_INSTANCE_

/* ========== Register definition for UART3 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_UART3_CR                    (0x40048000U) /**< \brief (UART3) Control Register */
  #define REG_UART3_MR                    (0x40048004U) /**< \brief (UART3) Mode Register */
  #define REG_UART3_IER                   (0x40048008U) /**< \brief (UART3) Interrupt Enable Register */
  #define REG_UART3_IDR                   (0x4004800CU) /**< \brief (UART3) Interrupt Disable Register */
  #define REG_UART3_IMR                   (0x40048010U) /**< \brief (UART3) Interrupt Mask Register */
  #define REG_UART3_SR                    (0x40048014U) /**< \brief (UART3) Status Register */
  #define REG_UART3_RHR                   (0x40048018U) /**< \brief (UART3) Receive Holding Register */
  #define REG_UART3_THR                   (0x4004801CU) /**< \brief (UART3) Transmit Holding Register */
  #define REG_UART3_BRGR                  (0x40048020U) /**< \brief (UART3) Baud Rate Generator Register */
#else
  #define REG_UART3_CR   (*(__O  uint32_t*)0x40048000U) /**< \brief (UART3) Control Register */
  #define REG_UART3_MR   (*(__IO uint32_t*)0x40048004U) /**< \brief (UART3) Mode Register */
  #define REG_UART3_IER  (*(__O  uint32_t*)0x40048008U) /**< \brief (UART3) Interrupt Enable Register */
  #define REG_UART3_IDR  (*(__O  uint32_t*)0x4004800CU) /**< \brief (UART3) Interrupt Disable Register */
  #define REG_UART3_IMR  (*(__I  uint32_t*)0x40048010U) /**< \brief (UART3) Interrupt Mask Register */
  #define REG_UART3_SR   (*(__I  uint32_t*)0x40048014U) /**< \brief (UART3) Status Register */
  #define REG_UART3_RHR  (*(__I  uint32_t*)0x40048018U) /**< \brief (UART3) Receive Holding Register */
  #define REG_UART3_THR  (*(__O  uint32_t*)0x4004801CU) /**< \brief (UART3) Transmit Holding Register */
  #define REG_UART3_BRGR (*(__IO uint32_t*)0x40048020U) /**< \brief (UART3) Baud Rate Generator Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM4N_UART3_INSTANCE_ */
