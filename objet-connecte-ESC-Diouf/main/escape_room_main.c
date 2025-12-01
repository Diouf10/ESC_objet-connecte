#include <stdio.h>
#include <string.h>
#include "led.h"
#include "push_button.h"
#include "buzzer.h"
#include "pir_sensor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "wifi.h"

#define SEQ_LEN 4
#define LED_VERTE GPIO_NUM_2
#define LED_ROUGE GPIO_NUM_4

static const char *TAG = "ESCAPE_ROOM";

QueueHandle_t event_queue;
int codeSecret[SEQ_LEN] = {1, 3, 2, 4};
int tentative[SEQ_LEN];
int index_seq = 0;

void iot_task(void *pvParameters)
{
    char json[64];

    while (1) {
        if (xQueueReceive(event_queue, &json, portMAX_DELAY)) {

            // On attend le WiFi AVANT d'envoyer
            xEventGroupWaitBits(
                wifi_event_group,
                WIFI_CONNECTED_BIT,
                pdFALSE,
                pdTRUE,
                portMAX_DELAY
            );

            ESP_LOGI("IoT", "Envoi JSON: %s", json);

            esp_http_client_config_t config = {
                .url = "http://10.0.0.87:3005/api/game/event",
                .method = HTTP_METHOD_POST,
                .transport_type = HTTP_TRANSPORT_OVER_TCP,
            };

            esp_http_client_handle_t client = esp_http_client_init(&config);
            esp_http_client_set_header(client, "Content-Type", "application/json");
            esp_http_client_set_post_field(client, json, strlen(json));

            esp_err_t err = esp_http_client_perform(client);
            if (err != ESP_OK) {
                ESP_LOGE("IoT", "HTTP POST failed: %s", esp_err_to_name(err));
            }

            esp_http_client_cleanup(client);
        }
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Initialisation du système...");

    wifi_init_sta();

    event_queue = xQueueCreate(10, sizeof(char) * 64);
    xTaskCreate(iot_task, "iot_task", 4096, NULL, 5, NULL);

    gpio_num_t btns[4] = {12, 13, 14, 15};
    buttons_init(btns, 4);

    gpio_reset_pin(LED_VERTE);
    gpio_set_direction(LED_VERTE, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_VERTE, 0);

    gpio_reset_pin(LED_ROUGE);
    gpio_set_direction(LED_ROUGE, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_ROUGE, 0);

    buzzer_init(GPIO_NUM_5);
    pir_init(GPIO_NUM_23);

    ESP_LOGI(TAG, "Système prêt. En attente de détection PIR...");

    int lastPirState = 0;
    bool pir_disabled = false;
    bool active_play = false;
    TickType_t pir_disabled_start = 0;

    while (1) {
        if (pir_disabled && (xTaskGetTickCount() - pir_disabled_start) >= pdMS_TO_TICKS(300000)) {
            pir_disabled = false;
            ESP_LOGI(TAG, "Le capteur PIR est réactivé.");
        }

        int pirState = 0;
        if (!pir_disabled) {
            int pirNow = pir_detected();
            vTaskDelay(pdMS_TO_TICKS(200));
            int pirConfirm = pir_detected();

            if (pirNow == 1 && pirConfirm == 1 && lastPirState == 0) {
                ESP_LOGI(TAG, "PIR détecte un joueur devant le système !");

                char json[64];
                snprintf(json, sizeof(json), "{\"event\":\"pir\",\"value\":1}");
                xQueueSend(event_queue, json, 0);

                gpio_set_level(LED_VERTE, 1);
                buzzer_beep_short();
                vTaskDelay(pdMS_TO_TICKS(300));
                gpio_set_level(LED_VERTE, 0);
                ESP_LOGI(TAG, "Système activé. En attente d'une entrée de code...");

                active_play = true;
                pir_disabled = true;
                pir_disabled_start = xTaskGetTickCount();
                ESP_LOGI(TAG, "PIR désactivé pour 5 minutes.");
            }

            pirState = pirNow;
            lastPirState = pirNow;
        }

        if (active_play) {
            int b = read_button();
            if (b != -1) {
                ESP_LOGI(TAG, "Bouton %d pressé", b);

                char json[64];
                snprintf(json, sizeof(json), "{\"event\":\"button\",\"value\":%d}", b);
                xQueueSend(event_queue, json, 0);

                tentative[index_seq] = b;
                index_seq++;

                buzzer_beep_short();

                if (index_seq == SEQ_LEN) {
                    ESP_LOGI(TAG, "Séquence complète entrée, vérification...");
                    int correct = 1;
                    for (int i = 0; i < SEQ_LEN; i++) {
                        ESP_LOGI(TAG, "Tentative[%d] = %d, CodeSecret[%d] = %d",
                                 i, tentative[i], i, codeSecret[i]);
                        if (tentative[i] != codeSecret[i]) {
                            correct = 0;
                            ESP_LOGW(TAG, "Erreur à la position %d : attendu %d, reçu %d",
                                     i, codeSecret[i], tentative[i]);
                            break;
                        }
                    }

                    if (correct) {
                        ESP_LOGI(TAG, "Succès ! Séquence correcte.");

                        char json[64];
                        snprintf(json, sizeof(json), "{\"event\":\"success\",\"value\":1}");
                        xQueueSend(event_queue, json, 0);

                        gpio_set_level(LED_VERTE, 1);
                        buzzer_play_victory();
                        vTaskDelay(pdMS_TO_TICKS(3000));
                        gpio_set_level(LED_VERTE, 0);
                    } else {
                        ESP_LOGE(TAG, "Échec ! Séquence incorrecte.");

                        char json[64];
                        snprintf(json, sizeof(json), "{\"event\":\"failure\",\"value\":1}");
                        xQueueSend(event_queue, json, 0);

                        gpio_set_level(LED_ROUGE, 1);
                        buzzer_beep_long();
                        vTaskDelay(pdMS_TO_TICKS(500));
                        gpio_set_level(LED_ROUGE, 0);
                    }

                    index_seq = 0;
                    ESP_LOGI(TAG, "Réinitialisation de la séquence, prêt pour un nouvel essai.");
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
