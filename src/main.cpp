#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "sharp_mip_display.h"

// SPI pins
#define SPI_SCK_PIN  8U
#define SPI_MOSI_PIN 10U
#define SPI_CS_PIN   7U

#define DISPLAY_WIDTH 400U
#define DISPLAY_HEIGHT 240U


int main() {
    stdio_init_all();
    sleep_ms(1000);

    // Configure SPI
    spi_init(spi0, 2'000'000);  // 2 MHz
    spi_set_format( spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    gpio_set_function(SPI_MOSI_PIN, GPIO_FUNC_SPI);  // TX
    gpio_set_function(SPI_SCK_PIN, GPIO_FUNC_SPI);  // SCK

    gpio_init(SPI_CS_PIN);
    gpio_set_dir(SPI_CS_PIN, GPIO_OUT);
    gpio_put(SPI_CS_PIN, 0);
    sleep_ms(10);

    SharpMipDisplay* display = new SharpMipDisplay(DISPLAY_WIDTH, DISPLAY_HEIGHT, spi0, SPI_CS_PIN);
    sleep_ms(1000);

    while(true) {
      display->ClearScreen();
      display->RefreshScreen(3,3);
      sleep_ms(1000);
      display->DrawHorizontalLine(3);
      display->RefreshScreen(3,3);
    }
    return 0;
}

