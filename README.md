# Baby Crying Detection System

> Developed as a module for the 'Smart Cradle' IoT Project at KAU in 2023

## Overview
This Arduino-based system, using an ESP32 microcontroller(`DOIT ESP32 Devkit V1`) and a sound sensor (`SZH-EK033`), detects when a baby is crying and communicates the detection status to AWS IoT Core. The system employs advanced sound pattern analysis and frequency analysis to accurately identify a baby's cry.

![제목 없는 디자인](https://github.com/Wendy-Nam/IoT-BabyCryDetection/assets/142412339/c6e6fdbc-da5e-445b-87f8-f025755ffb1c)

## AWS IoT Library
The system uses the AWS_IOT library from the `arduino-esp32/AWS_IOT` directory in the [ExploreEmbedded Hornbill examples](https://github.com/ExploreEmbedded/Hornbill-Examples) for AWS IoT Core integration. This library facilitates communication between the ESP32 device and AWS IoT Core, allowing for the real-time transmission of baby crying alerts.

## Core Logic
### Sound Detection
The system analyzes audio signals to detect crying patterns. The `detectSoundPattern()` function is key to identifying spikes in sound intensity, which are indicative of a baby's cry. This function uses the following parameters:
- `MIN_SPIKES`: Minimum number of spikes to consider a sound as a crying pattern.
- `MAX_SPIKES`: Maximum spike count to differentiate between a baby's cry and continuous noise.
- `SPIKE_THRESHOLD`: The threshold for detecting a spike in intensity.

### FFT Analysis
The `isBabyCrying()` function performs Fast Fourier Transform (FFT) to analyze the sound frequencies. It determines if these frequencies fall within the baby crying frequency range (defined by `BABY_CRY_FREQ_MIN` and `BABY_CRY_FREQ_MAX`) and exceed a set intensity threshold (`INTENSITY_THRESHOLD`). Key constants in this function are:
- `BABY_CRY_FREQ_MIN` and `BABY_CRY_FREQ_MAX`: Define the frequency range typical for a baby's cry.
- `INTENSITY_THRESHOLD`: The minimum intensity level to consider a sound as a baby's cry.
- `MATCH_THRESHOLD`: The number of frequency matches within the range to consider as a baby crying.

### Sound Level Measurement
The `readDecibelLevel()` function measures the sound level using the `SOUND_SENSOR_PIN`. The sound threshold (`SOUND_THRESHOLD`) determines if the sound level is high enough to be considered for further analysis.

## Configuration
Set your WiFi credentials (`ssid` and `password`) as per your setup. Adjust the sound sensor sensitivity and thresholds as required for your environment.

## Usage
The system operates continuously, analyzing ambient sounds. It uses the combination of sound pattern detection and FFT analysis to determine if a baby is crying. The results are printed on the Serial Monitor for observation.

## Notes
- Calibrate the `SOUND_THRESHOLD`, `SPIKE_THRESHOLD`, `INTENSITY_THRESHOLD`, and `MATCH_THRESHOLD` according to your specific environment and sensor placement.
- The system's accuracy depends on the correct calibration and placement of the sound sensor.
- This system was developed as part of the [Smart Cradle project](https://github.com/JeongJongMun/IOT-SmartBabyCradle), specifically to implement the baby crying detection logic used in the project.

    <img width="469" alt="스크린샷 2023-12-09 오후 7 05 07" src="https://github.com/Wendy-Nam/IoT-BabyCryDetection/assets/142412339/77f8f379-2b44-4040-af08-e7fb850dbb92">
