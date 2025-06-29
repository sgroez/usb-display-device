#include "pico/stdlib.h"
#include "hardware/spi.h"

#include "sharp_mip_display.h"

// Display size
#define WIDTH  400
#define HEIGHT 240

// SPI and control pins
#define SPI_PORT   spi0
#define BAUDRATE   2000000
#define PIN_SCK    2
#define PIN_MOSI   3
#define PIN_CS     1
#define PIN_DISP   0

int main() {
    stdio_init_all();

    spi_init(SPI_PORT, BAUDRATE);
    spi_set_format(SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1); // CS idle high

    gpio_init(PIN_DISP);
    gpio_set_dir(PIN_DISP, GPIO_OUT);
    gpio_put(PIN_DISP, 1); // Display ON

    auto display = new SharpMipDisplay(WIDTH, HEIGHT, SPI_PORT, PIN_CS);

    display->Init();
    display->Clear();

    // Draw border
    display->DrawHorizontalLine(0, 0, WIDTH);
    display->DrawHorizontalLine(0, HEIGHT - 1, WIDTH);
    display->DrawVerticalLine(0, 0, HEIGHT);
    display->DrawVerticalLine(WIDTH - 1, 0, HEIGHT);

    display->DrawLineOfText(1, 10, "Hello, Sharp!", kFont_16_20, false);

    display->Refresh();

    while (true) sleep_ms(1000);
}

