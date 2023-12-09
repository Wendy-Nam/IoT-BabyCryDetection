# Baby Crying Detection System

## 개요
ESP32 보드(`DOIT ESP32 Devkit V1`)와 사운드 센서(`SZH-EK033`)를 사용하여 아기의 울음소리를 감지하고, AWS IoT Core에 감지 상태를 전송하는 시스템.
사운드 패턴 분석 및 주파수 분석을 통해, 아기의 울음소리를 정확하게 식별하는 것을 목표로 함.

![설명 없음](https://github.com/Wendy-Nam/IoT-BabyCryDetection/assets/142412339/c6e6fdbc-da5e-445b-87f8-f025755ffb1c)

## AWS IoT 라이브러리
이 시스템은 [ExploreEmbedded Hornbill 예제](https://github.com/ExploreEmbedded/Hornbill-Examples)의 `arduino-esp32/AWS_IOT` 디렉토리에 있는 AWS_IOT 라이브러리를 사용하여 AWS IoT Core와 통신함.
이 라이브러리는 ESP32 장치에서 AWS IoT Core로 실시간으로 아기 울음 알림을 전송하는 것에 사용됨

## 핵심 로직

### 사운드 레벨 측정

`readDecibelLevel()` 함수는 `SOUND_SENSOR_PIN`을 사용하여 사운드 레벨을 측정.

사운드 임계값(`SOUND_THRESHOLD`)은 사운드 레벨이 충분히 높은지 결정. (만일 주변의 음성 강도가 높을 경우, 아기 울음소리 식별을 위한 분석을 시작함) 

### 울음 패턴 감지

이 시스템은 오디오 신호를 분석하여 울음 패턴을 감지함.

`detectSoundPattern()` 함수는 사운드 강도의 급격한 변화를 식별하는 역할을 함.

이 함수는 다음과 같은 매개변수를 사용:

- `MIN_SPIKES`: 울음 패턴으로 간주되는 최소 스파이크 수.
- `MAX_SPIKES`: 아기 울음소리와 지속적인 소음을 구별하기 위한 최대 스파이크 수.
- `SPIKE_THRESHOLD`: 스파이크 감지 임계값.

### FFT 분석

`isBabyCrying()` 함수는 Fast Fourier Transform(FFT)을 사용하여 사운드 주파수를 분석함.

이 함수는 주파수가 아기 울음소리의 전형적인 범위(`BABY_CRY_FREQ_MIN`에서 `BABY_CRY_FREQ_MAX`)에 해당하고 설정된 강도 임계값(`INTENSITY_THRESHOLD`)을 초과하는지 확인함.

이 함수에서 중요한 상수들은 다음과 같음:

- `BABY_CRY_FREQ_MIN` 및 `BABY_CRY_FREQ_MAX`: 아기 울음소리의 전형적인 주파수 범위를 정의.
- `INTENSITY_THRESHOLD`: 사운드를 아기 울음소리로 간주하기 위한 최소 강도 수준.
- `MATCH_THRESHOLD`: 울음으로 간주하기 위한 범위 내 주파수 일치 횟수.

## 구성

설정에 맞게 WiFi 자격 증명(`ssid` 및 `password`)을 설정할 필요가 있음.

환경에 따라 사운드 센서의 민감도 및 임계값을 조정해야함.

## 사용 방법

이 시스템은 지속적으로 주변 소리를 분석함.

사운드 패턴 감지 및 FFT 분석을 결합하여 아기가 우는지 여부를 결정함.

결과는 Serial Monitor에 출력됨 (해당 debug 코드에 대한 주석 해제 필요함)

## 참고 사항
- `SOUND_THRESHOLD`, `SPIKE_THRESHOLD`, `INTENSITY_THRESHOLD`, `MATCH_THRESHOLD`를 특정 환경 및 센서 배치에 따라 바꿀 필요가 있음.
- 시스템의 정확도는 올바른 보정 및 센서 배치에 따라 달라짐.
- 이 시스템은 [Smart Cradle 프로젝트](https://github.com/JeongJongMun/IOT-SmartBabyCradle)의 일부로 개발되었으며, 프로젝트에서 사용되는 아기 울음 감지 로직을 구현하기 위해 제작됨.

    <img width="469" alt="스크린샷 2023-12-09 오후 7 05 07" src="https://github.com/Wendy-Nam/IoT-BabyCryDetection/assets/142412339/77f8f379-2b44-4040-af08-e7fb850dbb92">
