#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <time.h>

#include "common.h"
#include "common_macros.h"
#include "sj2_cli.h"

#include "mp3.h"

xQueueHandle xSongQueue;
xQueueHandle xGameSoundsQueue;

void file_init() {
  FRESULT status;       // File status return structure
  DIR dir;              // Directory structure
  static FILINFO finfo; // File info structure

  status = f_opendir(&dir, ""); // Open directory ("1:" = SD card)
  // vTaskDelay(1000);

  if (status == FR_OK) // If directory open successfully
  {
    while (1) {
      status = f_readdir(&dir, &finfo); // Read filename and store in file info structure

      if (status != FR_OK || finfo.fname[0] == 0) // Exit on error or end of directory
      {
        break;
      }

      if (!(finfo.fattrib & AM_DIR)) // If not a sub-directory
      {
        SMACMAN__DEBUG_PRINTF("%s", finfo.fname); // Store "1:" + filename into song name buffer
        SMACMAN__DEBUG_PRINTF("%s", finfo.fname); // Store original filename into play list
      }
    }

    f_closedir(&dir); // Close directory
  } else {
    SMACMAN__DEBUG_PRINTF("Open Dir Failed\n");
  }
}

static void timer_software_interrupt(void *params) {
  while (1) {
    vTaskDelay(5000);
    xQueueSend(xGameSoundsQueue, 1, portMAX_DELAY);
    printf("Playing Sound\n");
  }
}

int main(void) {

  SMACMAN__DEBUG_PRINTF("Calling Mp3 Init\n");
  if (mp3__init()) {
    mp3__set_volume(10, 10);
    mp3__sine_test(0x01, 1000);
    SMACMAN__DEBUG_PRINTF("MP3 Initialized Successfully\n");
    common__init(&xSongQueue, &xGameSoundsQueue);
    file_init();
    // sdcard_handle__init();
  } else {
    SMACMAN__DEBUG_PRINTF("MP3 Not Initialized!\n");
  }

  xTaskCreate(sdcard_handle__read_song_task, "Read_Song", (512U * 8) / sizeof(void *), &xSongQueue, PRIORITY_MEDIUM,
              NULL);
  xTaskCreate(mp3__play_track_task, "Play_Song", (512U * 8) / sizeof(void *), &xSongQueue, PRIORITY_HIGH, NULL);
  // xTaskCreate(mp3__play_sounds_task, "Play_Sound", (512U * 8) / sizeof(void *), &xGameSoundsQueue, PRIORITY_MEDIUM,
  //             NULL);
  // xTaskCreate(timer_software_interrupt, "swi", (512U * 8) / sizeof(void *), &xGameSoundsQueue, PRIORITY_MEDIUM,
  // NULL);

#if 0
  // SMACMAN__DEBUG_PRINTF() takes more stack space, size this tasks' stack higher
  xTaskCreate(uart_task, "uart", (512U * 8) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
#else
  sj2_cli__init();
#endif

  puts("Starting RTOS");
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler runs out of memory and fails

  return 0;
}
