#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <SdFat.h>

#define SD_CS 10
#define BUFFER_LEN 2
#define SPI_SPEED SD_SCK_MHZ(4)
// #define SPI_SPEED SD_SCK_MHZ(50) // For when we have good connections lmao

SdFat32 sd;
File32 file;
File32 root;

Adafruit_BMP280 bmp;
Adafruit_MPU6050 mpu;

float ground_pressure;
unsigned long count = 0;

struct dataRow {
  unsigned long count, ms;
  float altitude, gyro_x, gyro_y, gyro_z, acc_x, acc_y, acc_z;
};

dataRow* buffer;
size_t buffer_ptr = 0;

sensors_event_t a, g, temp;

void setup() {
  Serial.begin(115200);
  while ( !Serial ) delay(100);   // wait for native usb
  buffer = (dataRow*) malloc(sizeof(dataRow) * BUFFER_LEN);

  bool failed_startup = false;

  // Connect to MPU
  if (!mpu.begin()) {
    failed_startup = true;
  } else {
    // Configure MPU settings
    mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  }

  delay(100);

  // Connect to BMP
  if (!bmp.begin()) {
    failed_startup = true;
  } else {
    // Configure BMP settings
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X8,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X4,      /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_63); /* Standby time. */
  }

  delay(100);

  // Connect to SD card
  if (!sd.begin(SD_CS, SPI_SPEED)) {
    if (sd.card()->errorCode()) {
      Serial.print("SD Error:");
      Serial.println(sd.card()->errorCode());
    }
    failed_startup = true;
  }

  delay(1000);

  // Count files in directory
  // if (!root.open("/")) {
  //   //Serial.println("Failed to open root directory");
  //   failed_startup |= true;
  // }
  // int rootFileCount = 0;
  // while (file.openNext(&root, O_RDONLY)) {
  //   rootFileCount++;
  //   file.close();
  // }
  // //Serial.print("Counted ");
  // //Serial.print(rootFileCount);
  // //Serial.println(" files");
  // root.close();

  // Open File
  char filename[20];
  sprintf(filename, "Log_%d.csv", 1);
  if (!file.open(filename, O_WRITE | O_CREAT)) {
    failed_startup |= true;
  } else {
    Serial.print("SD file '");
    Serial.print(filename);
    Serial.println("' Ready!");
  }

  // Take an initial pressure reading
  ground_pressure = bmp.readPressure();
  delay(100);
}

void readSensors(dataRow* ptr, unsigned long count) {
  ptr->count = count;
  ptr->ms = millis();

  ptr->altitude =  bmp.readAltitude(ground_pressure);

  mpu.getEvent(&a, &g, &temp);

  ptr->acc_x = a.acceleration.x;
  ptr->acc_y = a.acceleration.y;
  ptr->acc_z = a.acceleration.z;

  ptr->gyro_x = g.gyro.x;
  ptr->gyro_y = g.gyro.y;
  ptr->gyro_z = g.gyro.z;
}

void loop() {
  count++;
  readSensors(buffer + buffer_ptr, count);
  buffer_ptr++;
  if (buffer_ptr >= BUFFER_LEN) {
    buffer_ptr = 0;
    file.write((void*) buffer, sizeof(dataRow) * BUFFER_LEN);
    file.sync();
  }
  delay(100);
}