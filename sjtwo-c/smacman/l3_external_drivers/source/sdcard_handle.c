#include "sdcard_handle.h"
FIL fp;

bool open_file(char *filepath, FIL *fp1) {
  FRESULT ret_code = f_open(fp1, filepath, FA_READ);
  if (ret_code != FR_OK) {
    return false;
  }
  return true;
}

bool sdcard_handle__init() {

  if (open_file("song.mp3", &fp) == false) {
    SMACMAN__DEBUG_PRINTF("File Not Opened\n");
    return false;
  } else {
    SMACMAN__DEBUG_PRINTF("File opened\n");
    return true;
  }
}

// bool file_init() {
//   signal_for_mp3 = xSemaphoreCreateBinary();
//   mp3_data_queue = xQueueCreate(DATA_SIZE, sizeof(uint8_t));
//   if (open_file("song.mp3", &fp) == false) {
//     SMACMAN__DEBUG_PRINTF("File Not Opened\n");
//   } else {
//     SMACMAN__DEBUG_PRINTF("File opened\n");
//   }
// }

// bool mp3_read_from_sd() { return read_data_from_sd_and_put_in_queue(&fp); }

void close_file() { f_close(&fp); }

void sdcard_handle__read_song_task(void *songQueueHandle) {
  xQueueHandle *xSongQueue = (xQueueHandle *)songQueueHandle;
  uint8_t data_buffer[VS1053_DATABUFFERLEN];
  uint32_t bytes_read = VS1053_DATABUFFERLEN;
  FIL mySong;
  while (1) {
    FRESULT ret_code = f_open(&mySong, "pacman_death.wav", FA_READ);
    if (ret_code != FR_OK) {
      SMACMAN__DEBUG_PRINTF("File Not Opened %i\n", ret_code);
      fprintf(stderr, "File Not Opened %i\n", ret_code);
      if (ret_code == 3) {
        vTaskDelay(1000);
      }
    } else {
      bytes_read = VS1053_DATABUFFERLEN;
      SMACMAN__DEBUG_PRINTF("File opened\n");

      while (!mp3__ready_for_data())
        ;
      while (!(bytes_read < VS1053_DATABUFFERLEN)) {

        FRESULT f_read_ret_code = f_read(&mySong, data_buffer, VS1053_DATABUFFERLEN, &bytes_read);
        if (f_read_ret_code != FR_OK) {
          SMACMAN__DEBUG_PRINTF("File not Read %i\n", f_read_ret_code);
        }
        // for (int i = 0; i < bytes_read; i++)
        //   xQueueSend(*xSongQueue, &data_buffer[i], portMAX_DELAY);
        xQueueSend(*xSongQueue, data_buffer, portMAX_DELAY);
        SMACMAN__DEBUG_PRINTF("Data Sent to queue\n");
      }
      SMACMAN__DEBUG_PRINTF("Closing File\n");

      f_close(&mySong);
    }
  }
  //   return true;
}

// void read_dir() {
//   FRESULT status;       // File status return structure
//   DIR dir;              // Directory structure
//   static FILINFO finfo; // File info structure

//   status = f_opendir(&dir, "/"); // Open directory ("1:" = SD card)

//   if (status == FR_OK) // If directory open successfully
//   {
//     // while (1)
//     {
//       status = f_readdir(&dir, &finfo); // Read filename and store in file info structure

//       if (status != FR_OK || finfo.fname[0] == 0) // Exit on error or end of directory
//       {
//         SMACMAN__DEBUG_PRINTF("Error Reading Directory\n");
//         // break;
//       }

//       if (!(finfo.fattrib & AM_DIR)) // If not a sub-directory
//       {
//         SMACMAN__DEBUG_PRINTF("%s\n", finfo.fname);
//         // sprintf(fileName[totalFileNumber], "1:%s", finfo.fname); // Store "1:" + filename into song name buffer
//         // sprintf(playlist[totalFileNumber], "%s", finfo.fname); // Store original filename into play list
//         // totalFileNumber++;
//       }
//     }

//     f_closedir(&dir); // Close directory
//   } else {
//     SMACMAN__DEBUG_PRINTF("Directory Not Open\n");
//   }
// }