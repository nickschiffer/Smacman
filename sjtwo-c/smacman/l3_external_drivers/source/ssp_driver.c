#include "ssp_driver.h"
#include "clock.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include <stdio.h>

/**
 * These are the DMA request numbers that the hardware maps
 * See LPC40xx user manual, chapter: General Purpose DMA controller, Table 696: DMA connections
 */
#define SSP2__DMA_REQUEST_TX 6UL
#define SSP2__DMA_REQUEST_RX 7UL

/**
 * There are only 8 DMA channels on the LPC40xx so confirm validity
 */
#if !(SSP2__DMA_TX_CHANNEL >= 0 && SSP2__DMA_TX_CHANNEL <= 7)
#error "SSP2__DMA_TX_CHANNEL must be between 0 and 7"
#endif
#if !(SSP2__DMA_RX_CHANNEL >= 0 && SSP2__DMA_RX_CHANNEL <= 7)
#error "SSP2__DMA_RX_CHANNEL must be between 0 and 7"
#endif

typedef enum {
  SSP_DMA__ERROR_NONE = 0,
  SSP_DMA__ERROR_LENGTH = 1,
  SSP_DMA__ERROR_BUSY = 2,
} ssp_dma_error_e;

/**
 * Initialize the DMA struct pointers that we will use
 */
static const size_t ssp2__dma_channel_memory_spacing = (LPC_GPDMACH1_BASE - LPC_GPDMACH0_BASE);
static LPC_GPDMACH_TypeDef *ssp2__dma_tx =
    (LPC_GPDMACH_TypeDef *)(LPC_GPDMACH0_BASE + (SSP2__DMA_TX_CHANNEL * ssp2__dma_channel_memory_spacing));
static LPC_GPDMACH_TypeDef *ssp2__dma_rx =
    (LPC_GPDMACH_TypeDef *)(LPC_GPDMACH0_BASE + (SSP2__DMA_RX_CHANNEL * ssp2__dma_channel_memory_spacing));

void c_ssp2__init(uint32_t max_clock_mhz) {
  // Refer to LPC User manual and setup the register bits correctly
  // a) Power on Peripheral
  LPC_SC->PCONP |= (1U << 20);

  // b) Setup control registers CR0 and CR1
  LPC_SSP2->CR0 = (7 << 0);
  LPC_SSP2->CR1 = (1 << 1);
  // c) Setup prescalar register to be <= max_clock_mhz
  uint32_t divider = 2;
  const uint32_t max_cpu_clock_mhz = clock__get_core_clock_hz();

  while (((max_cpu_clock_mhz / divider) > max_clock_mhz) && (254 >= divider)) {
    divider += 2;
  }

  LPC_SSP2->CPSR = divider;
}

uint8_t c_ssp2__exchange_byte(uint8_t data_out) {
  LPC_SSP2->DR = data_out;

  while (LPC_SSP2->SR & (1 << 4)) {
    ; // Wait until Tx FIFO is not empty
  }
  uint8_t data_received = (uint8_t)(LPC_SSP2->DR & 0xFF);
  return data_received;
}

void c_ssp2__dma_init(void) {
  lpc_peripheral__turn_on_power_to(LPC_PERIPHERAL__GPDMA);

  const uint32_t enable_bitmask = (1 << 0);
  LPC_GPDMA->Config = enable_bitmask;
  while (!(LPC_GPDMA->Config & enable_bitmask)) {
    ; // Wait for DMA to be enabled
  }

  // SSP dma channels will not use linked list transfers
  ssp2__dma_rx->CLLI = 0;
  ssp2__dma_tx->CLLI = 0;

  // These registers are only initialized once, but this optimization makes no measurable difference
  // ssp2__dma_tx->CDestAddr = (uint32_t)(&(LPC_SSP2->DR));
  // ssp2__dma_rx->CSrcAddr = (uint32_t)(&(LPC_SSP2->DR));
}
