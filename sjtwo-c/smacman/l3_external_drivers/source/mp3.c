#include "mp3.h"

gpio_s XDCS, CS, RST, DREQ;

static QueueHandle_t mp3_data_queue;
static xSemaphoreHandle signal_for_mp3;

static FIL fp;

void mp3_pins_init() {
  XDCS = gpio__construct_with_function(4, 28, GPIO__FUNCITON_0_IO_PIN);
  CS = gpio__construct_with_function(4, 29, GPIO__FUNCITON_0_IO_PIN);
  RST = gpio__construct_with_function(0, 6, GPIO__FUNCITON_0_IO_PIN);
  DREQ = gpio__construct_with_function(0, 7, GPIO__FUNCITON_0_IO_PIN);
  gpio__set_as_output(XDCS);
  gpio__set_as_output(CS);
  gpio__set_as_output(RST);
  gpio__set_as_input(DREQ);

  ssp2__initialize(24 * 1000);
  write_to_sci_reg(SCI_CLK_REG, 0x20, 0x00);
  write_to_sci_reg(SCI_VOL_REG, 0x00, 0x00);
}

void wait_for_dreq(void) {
  while (!gpio__get(DREQ)) {
    vTaskDelay(1);
  }
}

void write_to_sci_reg(char addr, char msb, char lsb) {
  wait_for_dreq();
  gpio__reset(CS);
  ssp2__exchange_byte(SCI_WRITE_OPCODE);
  ssp2__exchange_byte(addr);
  ssp2__exchange_byte(msb);
  ssp2__exchange_byte(lsb);
  gpio__set(CS);
}

void send_sdi_byte(uint8_t byte) { ssp2__exchange_byte(byte); }

void set_xdcs_low() { gpio__reset(XDCS); }

void set_xdcs_high() { gpio__set(XDCS); }

bool mp3_init() {
  mp3_pins_init();
  //   mp3_data_queue = xQueueCreate(DATA_SIZE, sizeof(uint8_t));
  //   read_dir();
  //   if (open_file("song.mp3", &fp)) {
  file_init();
}

bool file_init() {
  signal_for_mp3 = xSemaphoreCreateBinary();
  mp3_data_queue = xQueueCreate(DATA_SIZE, sizeof(uint8_t));
  if (open_file("song.mp3", &fp) == false) {
    SMACMAN__DEBUG_PRINTF("File Not Opened\n");
  } else {
    SMACMAN__DEBUG_PRINTF("File opened\n");
  }
}

bool mp3_read_from_sd() { return read_data_from_sd_and_put_in_queue(&fp); }

bool open_file(char *filepath, FIL *fp1) {
  FRESULT ret_code = f_open(fp1, filepath, FA_READ);
  if (ret_code != FR_OK) {
    return false;
  }
  return true;
}

void close_file() { f_close(&fp); }

bool read_data_from_sd_and_put_in_queue(FIL *fp1) {
  uint8_t data_buffer[DATA_SIZE];
  uint32_t bytes_read = 0;
  FRESULT ret_code = f_read(&fp, (void *)data_buffer, DATA_SIZE, &bytes_read);
  if (ret_code != FR_OK) {
    SMACMAN__DEBUG_PRINTF("Data Not Read Properly\n");
    close_file();
    // vTaskDelay(10);
    open_file("song.mp3", &fp);
    return false;
  } else {
    if (bytes_read > 0) {
      // xSemaphoreGive(signal_for_mp3);
      xQueueSend(mp3_data_queue, &data_buffer, portMAX_DELAY);
      SMACMAN__DEBUG_PRINTF("Data Sent to Queue = %s\n", data_buffer);
    }
  }
  return true;
}

void mp3_play_song_from_queue() {
  uint8_t data_buffer[DATA_SIZE];
  // if(xSemaphoreTake(signal_for_mp3)){
  if (xQueueReceive(mp3_data_queue, &data_buffer, portMAX_DELAY)) {
    for (int i = 0; i < DATA_SIZE; i += MP3_PROCESSING_SIZE) {
      wait_for_dreq();
      set_xdcs_low();
      for (int j = 0; j < MP3_PROCESSING_SIZE; j++) {
        SMACMAN__DEBUG_PRINTF(".");
        send_sdi_byte(data_buffer[i + j]);
      }
      set_xdcs_high();
    }
  }
  // }
}

void read_dir() {
  FRESULT status;       // File status return structure
  DIR dir;              // Directory structure
  static FILINFO finfo; // File info structure

  status = f_opendir(&dir, "1:"); // Open directory ("1:" = SD card)

  if (status == FR_OK) // If directory open successfully
  {
    // while (1)
    {
      status = f_readdir(&dir, &finfo); // Read filename and store in file info structure

      if (status != FR_OK || finfo.fname[0] == 0) // Exit on error or end of directory
      {
        SMACMAN__DEBUG_PRINTF("Error Reading Directory\n");
        // break;
      }

      if (!(finfo.fattrib & AM_DIR)) // If not a sub-directory
      {
        SMACMAN__DEBUG_PRINTF("%s\n", finfo.fname);
        // sprintf(fileName[totalFileNumber], "1:%s", finfo.fname); // Store "1:" + filename into song name buffer
        // sprintf(playlist[totalFileNumber], "%s", finfo.fname); // Store original filename into play list
        // totalFileNumber++;
      }
    }

    f_closedir(&dir); // Close directory
  } else {
    SMACMAN__DEBUG_PRINTF("Directory Not Open\n");
  }
}

uint16_t sci_read(uint8_t addr) {
  uint16_t data;

  // cs_.setLow();
  gpio__reset(CS);
  ssp2__exchange_byte(SCI_READ_OPCODE);
  ssp2__exchange_byte(addr);

  // delay_us(10);
  data = ssp2__exchange_byte(0x00);
  data <<= 8;
  data |= ssp2__exchange_byte(0x00);
  gpio__set(CS);
  return data;
}