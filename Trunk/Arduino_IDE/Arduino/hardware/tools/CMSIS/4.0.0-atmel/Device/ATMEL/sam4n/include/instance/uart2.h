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

#ifndef _SAM4N_UART2_INSTANCE_
#define _SAM4N_UART2_INSTANCE_

/* ========== Register definition for UART2 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_UART2_CR                    (0x40044000U) /**< \brief (UART2) Control Register */
  #define REG_UART2_MR                    (0x40044004U) /**< \brief (UART2) Mode Register */
  #define REG_UART2_IER                   (0x40044008U) /**< \brief (UART2) Interrupt Enable Register */
  #define REG_UART2_IDR                   (0x4004400CU) /**< \brief (UART2) Interrupt Disable Register */
  #define REG_UART2_IMR                   (0x40044010U) /**< \brief (UART2) Interrupt Mask Register */
  #define REG_UART2_SR                    (0x40044014U) /**< \brief (UART2) Status Register */
  #define REG_UART2_RHR                   (0x40044018U) /**< \brief (UART2) Receive Holding Register */
  #define REG_UART2_THR                   (0x4004401CU) /**< \brief (UART2) Transmit Holding Register */
  #define REG_UART2_BRGR                  (0x40044020U) /**< \brief (UART2) Baud Rate Generator Register */
  #define REG_UART2_RPR                   (0x40044100U) /**< \brief (UART2) Receive Pointer Register */
  #define REG_UART2_RCR                   (0x40044104U) /**< \brief (UART2) Receive Counter Register */
  #define REG_UART2_TPR                   (0x40044108U) /**< \brief (UART2) Transmit Pointer Register */
  #define REG_UART2_TCR                   (0x4004410CU) /**< \brief (UART2) Transmit Counter Register */
  #define REG_UART2_RNPR                  (0x40044110U) /**< \brief (UART2) Receive Next Pointer Register */
  #define REG_UART2_RNCR                  (0x40044114U) /**< \brief (UART2) Receive Next Counter Register */
  #define REG_UART2_TNPR                  (0x40044118U) /**< \brief (UART2) Transmit Next Pointer Register */
  #define REG_UART2_TNCR                  (0x4004411CU) /**< \brief (UART2) Transmit Next Counter Register */
  #define REG_UART2_PTCR                  (0x40044120U) /**< \brief (UART2) Transfer Control Register */
  #define REG_UART2_PTSR                  (0x40044124U) /**< \brief (UART2) Transfer Status Register */
#else
  #define REG_UART2_CR   (*(__O  uint32_t*)0x40044000U) /**< \brief (UART2) Control Register */
  #define REG_UART2_MR   (*(__IO uint32_t*)0x40044004U) /**< \brief (UART2) Mode Register */
  #define REG_UART2_IER  (*(__O  uint32_t*)0x40044008U) /**< \brief (UART2) Interrupt Enable Register */
  #define REG_UART2_IDR  (*(__O  uint32_t*)0x4004400CU) /**< \brief (UART2) Interrupt Disable Register */
  #define REG_UART2_IMR  (*(__I  uint32_t*)0x40044010U) /**< \brief (UART2) Interrupt Mask Register */
  #define REG_UART2_SR   (*(__I  uint32_t*)0x40044014U) /**< \brief (UART2) Status Register */
  #define REG_UART2_RHR  (*(__I  uint32_t*)0x40044018U) /**< \brief (UART2) Receive Holding Register */
  #define REG_UART2_THR  (*(__O  uint32_t*)0x4004401CU) /**< \brief (UART2) Transmit Holding Register */
  #define REG_UART2_BRGR (*(__IO uint32_t*)0x40044020U) /**< \brief (UART2) Baud Rate Generator Register */
  #define REG_UART2_RPR  (*(__IO uint32_t*)0x40044100U) /**< \brief (UART2) Receive Pointer Register */
  #define REG_UART2_RCR  (*(__IO uint32_t*)0x40044104U) /**< \brief (UART2) Receive Counter Register */
  #define REG_UART2_TPR  (*(__IO uint32_t*)0x40044108U) /**< \brief (UART2) Transmit Pointer Register */
  #define REG_UART2_TCR  (*(__IO uint32_t*)0x4004410CU) /**< \brief (UART2) Transmit Counter Register */
  #define REG_UART2_RNPR (*(__IO uint32_t*)0x40044110U) /**< \brief (UART2) Receive Next Pointer Register */
  #define REG_UART2_RNCR (*(__IO uint32_t*)0x40044114U) /**< \brief (UART2) Receive Next Counter Register */
  #define REG_UART2_TNPR (*(__IO uint32_t*)0x40044118U) /**< \brief (UART2) Transmit Next Pointer Register */
  #define REG_UART2_TNCR (*(__IO uint32_t*)0x4004411CU) /**< \brief (UART2) Transmit Next Counter Register */
  #define REG_UART2_PTCR (*(__O  uint32_t*)0x40044120U) /**< \brief (UART2) Transfer Control Register */
  #define REG_UART2_PTSR (*(__I  uint32_t*)0x40044124U) /**< \brief (UART2) Transfer Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM4N_UART2_INSTANCE_ */
