#include <arduinoFFT.h>
#include <WiFi.h>
#include <AWS_IOT.h>
#include <Arduino_JSON.h>

AWS_IOT awsIot;
const char* ssid = ""; // Enter your WiFi SSID
const char* password = ""; // Enter your WiFi password

char* HOST_ADDRESS = ""; // Enter your AWS IoT endpoint address
char* CLIENT_ID = ""; // Enter your AWS IoT client ID
char* BABY_CRYING_TOPIC = ""; // Enter your AWS IoT topic

const int SOUND_SENSOR_PIN = 34;
const int SAMPLES = 512; // Must be a power of 2
const double SAMPLING_FREQUENCY = 1000; // Hz, must be less than 10000 due to ADC
const int SOUND_THRESHOLD = 45; // Example threshold in decibel

// Constants for detectSoundPattern
const int MIN_SPIKES = 75; // Minimum number of spikes to consider for a crying pattern
const int MAX_SPIKES = 160; // Maximum number to avoid continuous noise
const double SPIKE_THRESHOLD = 15.0; // Threshold for detecting a spike in intensity

// Constants for isBabyCrying
const double BABY_CRY_FREQ_MIN = 250.0; // Minimum frequency for baby cry (in Hz)
const double BABY_CRY_FREQ_MAX = 550.0; // Maximum frequency for baby cry (in Hz)
const double INTENSITY_THRESHOLD = 2000.0; // Minimum intensity to consider
const int MATCH_THRESHOLD = 100; // Number of matches within the frequency range to consider as baby crying

int status = WL_IDLE_STATUS;
char payload[512];
unsigned long preMil = 0;
const long intMil = 5000; 

double vReal[SAMPLES];
double vImag[SAMPLES];
arduinoFFT FFT(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY);
bool haveCried = false;

double readDecibelLevel() {
    int analogValue = analogRead(SOUND_SENSOR_PIN);
    // double decibelLevel = (analogValue / 1023.0) * 120.0; // Convert to decibel (scaling)
    return double(analogValue);
}

void publishToAWSShadow(bool isCrying) {
  // JSON 객체를 생성합니다.
  JSONVar reportedState;
  reportedState["state"]["reported"]["isCrying"] = isCrying ? "ON" : "OFF";

  // JSON 객체를 문자열로 변환합니다.
  String shadowUpdate = JSON.stringify(reportedState);
  sprintf(payload, shadowUpdate.c_str());
  // 문자열을 char* 타입으로 변환합니다.

  // AWS IoT 쉐도우에 상태를 발행합니다.
  if(awsIot.publish(BABY_CRYING_TOPIC, payload) == 0) {
    Serial.println("Shadow update published successfully.");
  } else {
    Serial.println("Shadow update failed to publish.");
  }
}

bool detectSoundPattern() {
    double lastIntensity = 0.0;
    int spikeCount = 0;
    bool isSpike = false;

    for (int i = 0; i < SAMPLES; i++) {
        vReal[i] = analogRead(SOUND_SENSOR_PIN);
        double currentIntensity = vReal[i];

        if (!isSpike && currentIntensity > lastIntensity + SPIKE_THRESHOLD) {
            isSpike = true;
        } else if (isSpike && currentIntensity < lastIntensity) {
            spikeCount++;
            isSpike = false;
        }
        lastIntensity = currentIntensity;
        delay(20);
    }
    Serial.print("Spike Count: "); // Uncomment for debugging
    Serial.println(spikeCount);
    return spikeCount >= MIN_SPIKES && spikeCount <= MAX_SPIKES;
}

bool isBabyCrying(double frequencies[], double intensities[], int count) {
    int targetFrequencyMatches = 0;
    for (int i = 0; i < count; i++) {
        if (frequencies[i] >= BABY_CRY_FREQ_MIN && frequencies[i] <= BABY_CRY_FREQ_MAX &&
            intensities[i] > INTENSITY_THRESHOLD) {
            Serial.print("Frequency : "); // Uncomment for debugging
            Serial.print(frequencies[i]);
            Serial.print("   intensity : "); // Uncomment for debugging
            Serial.println(intensities[i]);
            targetFrequencyMatches++;
        }
    }
    Serial.print("Target Frequency Matches: "); // Uncomment for debugging
    Serial.println(targetFrequencyMatches);
    return targetFrequencyMatches > MATCH_THRESHOLD;
}

void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad) // 인터럽트 시 호출되는 함수
{
    strncpy(rcvdPayload,payLoad,payloadLen);
    rcvdPayload[payloadLen] = 0;
    msgReceived = 1;
}

void setup() {
    Serial.begin(115200);
    Serial.print("WIFI status = ");
    Serial.println(WiFi.getMode());
    WiFi.disconnect(true);
    delay(1000);
    WiFi.mode(WIFI_STA);
    delay(1000);
    Serial.print("WIFI status = ");
    Serial.println(WiFi.getMode()); //++choi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to wifi");
    if(awsIot.connect(HOST_ADDRESS,CLIENT_ID)== 0) {
        Serial.println("Connected to AWS");
        delay(1000);
        if(0==awsIot.subscribe(BABY_CRYING_TOPIC,mySubCallBackHandler)) {
            Serial.println("Subscribe Successfull");
        }
        else {
            Serial.println("Subscribe Failed, Check the Thing Name and Certificates");
            while(1);
        }
    }
    else {
        Serial.println("AWS connection failed, Check the HOST Address");
        while(1);
    }
    pinMode(SOUND_SENSOR_PIN,INPUT);
    delay(1000);
}

void loop() {
    double decibelLevel = readDecibelLevel();
    Serial.print("Decibel Level: "); // Uncomment for debugging
    Serial.println(decibelLevel);
  
    if (decibelLevel > SOUND_THRESHOLD) {
        Serial.println("Sound level above the threshold detected.");
        if (detectSoundPattern()) {
            // Perform FFT and intensity analysis
            FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
            FFT.Compute(FFT_FORWARD);
            FFT.ComplexToMagnitude();

            double frequencies[SAMPLES / 2];
            double intensities[SAMPLES / 2];
            for (int i = 0; i < SAMPLES / 2; i++) {
                frequencies[i] = (i * SAMPLING_FREQUENCY) / SAMPLES;
                intensities[i] = vReal[i];
            }
            if (isBabyCrying(frequencies, intensities, SAMPLES / 2)) {
              haveCried = true;
              Serial.println("Baby is crying! Updating shadow...");
              publishToAWSShadow(true);
            } else {
              if (haveCried) {
                publishToAWSShadow(false);
                haveCried = false;
              }
              Serial.println("No baby crying detected. Updating shadow...");
            }
        } else {
            if (haveCried) {
              publishToAWSShadow(false);
              haveCried = false;
            }
            Serial.println("No significant crying pattern detected.");
        }
    } else {
        // Serial.println("Sound level is below threshold."); // Uncomment for debugging
    }
    delay(1000);
}
