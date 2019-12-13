#include "mp3.h"

static gpio_s XDCS, CS, RST, DREQ;

void mp3__set_xdcs_low() { gpio__reset(XDCS); }

void mp3__set_xdcs_high() { gpio__set(XDCS); }

void mp3__set_dcs(bool s) {
  if (s == false) {
    mp3__set_xdcs_low();
  } else {
    mp3__set_xdcs_high();
  }
}

bool mp3__ready_for_data(void) {
  // while (!gpio__get(DREQ)) {
  //   vTaskDelay(1);
  // }
  return gpio__get(DREQ);
}

// bool mp3__ready_for_data() { return gpio__get(DREQ); }

void mp3__sci_write(uint8_t addr, uint16_t data) {
  gpio__reset(CS);
  c_ssp2__exchange_byte(VS1053_SCI_WRITE);
  c_ssp2__exchange_byte(addr);
  c_ssp2__exchange_byte(data >> 8);
  c_ssp2__exchange_byte(data & 0xFF);
  gpio__set(CS);
}

uint16_t mp3__sci_read(uint8_t addr) {
  uint16_t data;

  gpio__reset(CS);
  c_ssp2__exchange_byte(VS1053_SCI_READ);
  c_ssp2__exchange_byte(addr);

  // delay_us(10);
  data = c_ssp2__exchange_byte(0xFF);
  data <<= 8;
  data |= c_ssp2__exchange_byte(0xFF);
  gpio__set(CS);
  return data;
}

void mp3__reset(void) {
  gpio__reset(RST);
  delay__ms(100);
  gpio__set(RST);
  gpio__set(CS);
  gpio__set(XDCS);
  delay__ms(100);
  mp3__sci_write(VS1053_REG_MODE, VS1053_MODE_SM_SDINEW | VS1053_MODE_SM_RESET);
  delay__ms(200);
  printf("Waiting for dreq\n");
  while (!mp3__ready_for_data())
    ;
  mp3__sci_write(VS1053_REG_CLOCKF, 0x6000);
}

void mp3__set_volume(uint8_t left, uint8_t right) {
  uint16_t vol;
  vol = left;
  vol <<= 8;
  vol |= right;
  mp3__sci_write(VS1053_REG_VOLUME, vol);
}

void mp3__set_bass_treble(uint8_t b, uint8_t t) {
  uint16_t setting;
  setting = (15 | (b << 4) | (1 << 8) | (t << 12));
  mp3__sci_write(VS1053_REG_BASS, setting);
}

void mp3__init_song() {
  mp3__sci_write(VS1053_REG_MODE, VS1053_MODE_SM_LINE1 | VS1053_MODE_SM_SDINEW);
  mp3__sci_write(VS1053_REG_WRAMADDR, 0xC017);
  mp3__sci_write(VS1053_REG_WRAM, 3);
  mp3__sci_write(VS1053_REG_WRAMADDR, 0xC019);
  mp3__sci_write(VS1053_REG_WRAM, 0);
  mp3__sci_write(VS1053_REG_DECODETIME, 0x00);
  mp3__sci_write(VS1053_REG_DECODETIME, 0x00);
}

void mp3__send_data(uint8_t *buffer, uint16_t buffsize) {
  while (!mp3__ready_for_data())
    ;
  mp3__set_xdcs_low();

  // for (uint8_t i = 0; i < buffsize; i++) {
  //   if ((buffsize % 32) == 0) {
  //     while (!mp3__ready_for_data())
  //       ;
  //   }
  //   fprintf(stderr, "%x\n", buffer[i]);
  //   c_ssp2__exchange_byte(buffer[i]);
  // }
  // printf("End of chunk\n");

  while (buffsize--) {
    // printf("%d\n", buffsize);
    // printf("%x\n", buffer[0]);
    if ((buffsize % 32) == 0) {
      while (!mp3__ready_for_data())
        ;
    }
    c_ssp2__exchange_byte(buffer[0]);
    buffer++;
  }

  mp3__set_xdcs_high();
}

void mp3__sine_test(uint8_t n, uint16_t ms) {
  uint16_t mode = mp3__sci_read(VS1053_REG_MODE);
  mode |= 0x0020;
  mp3__sci_write(VS1053_REG_MODE, mode);

  while (!mp3__ready_for_data())
    ;

  gpio__reset(XDCS);
  c_ssp2__exchange_byte(0x53);
  c_ssp2__exchange_byte(0xEF);
  c_ssp2__exchange_byte(0x6E);
  c_ssp2__exchange_byte(n);
  c_ssp2__exchange_byte(0x00);
  c_ssp2__exchange_byte(0x00);
  c_ssp2__exchange_byte(0x00);
  c_ssp2__exchange_byte(0x00);
  gpio__set(XDCS);

  delay__ms(ms);

  gpio__reset(XDCS);
  c_ssp2__exchange_byte(0x45);
  c_ssp2__exchange_byte(0x78);
  c_ssp2__exchange_byte(0x69);
  c_ssp2__exchange_byte(0x74);
  c_ssp2__exchange_byte(0x00);
  c_ssp2__exchange_byte(0x00);
  c_ssp2__exchange_byte(0x00);
  c_ssp2__exchange_byte(0x00);
  gpio__set(XDCS);
}

static void mp3__pins_init() {
  XDCS = gpio__construct_with_function(4, 28, GPIO__FUNCITON_0_IO_PIN);
  CS = gpio__construct_with_function(4, 29, GPIO__FUNCITON_0_IO_PIN);
  RST = gpio__construct_with_function(0, 6, GPIO__FUNCITON_0_IO_PIN);
  DREQ = gpio__construct_with_function(0, 7, GPIO__FUNCITON_0_IO_PIN);
  gpio__set_as_output(XDCS);
  gpio__set_as_output(CS);
  gpio__set_as_output(RST);
  gpio__set_as_input(DREQ);
  gpio__set(XDCS);
  gpio__set(CS);
  gpio__reset(RST);
}

bool mp3__init() {
  // ssp2__initialize(12 * 1000);
  c_ssp2__init(8);
  mp3__pins_init();
  mp3__reset();
  uint8_t v = (mp3__sci_read(VS1053_REG_STATUS) >> 4) & 0x0F;
  SMACMAN__DEBUG_PRINTF("Version =  %u\n", v);
  return (v == 4);
}

void mp3__play_sounds_task(void *xGameSoundsQueueHandle) {
  xQueueHandle *xGameSoundsQueue = (xQueueHandle *)xGameSoundsQueueHandle;
  int which_sound;
  while (1) {
    xQueueReceive(*xGameSoundsQueue, &which_sound, portMAX_DELAY);
    switch (which_sound) {
    case 1:
    default:
      mp3__reset();
      mp3__sine_test(0x01, 1000);
      // vTaskDelay(1000);
      mp3__reset();
      break;
    }
    printf("Played Sound\n");
  }
}

void mp3__play_track_task(void *songQueueHandle) {
  xQueueHandle *xSongQueue = (xQueueHandle *)songQueueHandle;
  uint8_t songBuff[VS1053_DATABUFFERLEN];
  mp3__init_song();
  while (1) {
    // if(xQueueReceive(xVolumeQueue, &re_state, 0)) {
    //     if(re_state == kLEFT_TURN) {
    //         if(vol < minVol)
    //             vol += 2;
    //         myPlayer.setVolume(vol, vol);
    //     }
    //     else if(re_state == kRIGHT_TURN) {
    //         if (vol > maxVol)
    //             vol -= 2;
    //         myPlayer.setVolume(vol, vol);
    //     }
    // }
    // if (isBT) {
    //     myPlayer.setBassTreble(bass, treble);
    //     isBT = 0;
    // }
    xQueueReceive(*xSongQueue, songBuff, portMAX_DELAY);
    mp3__send_data(songBuff, VS1053_DATABUFFERLEN);
    SMACMAN__DEBUG_PRINTF("Data Received in Queue = %s\n", songBuff);
    // printf("Data Received in Queue = %s\n", songBuff);
    vTaskDelay(15);
  }
}
