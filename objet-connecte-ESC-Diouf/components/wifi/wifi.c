#include "wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"

// SSID et mot de passe lus depuis menuconfig (plus sécuritaire) (on évitee d'hardcoder le wifi ssid  et le mdp dans le code.)
#define WIFI_SSID CONFIG_PROJECT_WIFI_SSID
#define WIFI_PASS CONFIG_PROJECT_WIFI_PASS

static const char *TAG = "wifi";

// Event group utilisé pour signaler que le WiFi est connecté
EventGroupHandle_t wifi_event_group;

// Gestionnaire d'événements système pour le Wi-Fi
static void event_handler(void *arg,
                          esp_event_base_t event_base,
                          int32_t event_id,
                          void *event_data)
{
    // Lancement du Wi-Fi -> on se connecte
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    }

    // Déconnexion -> nouvelle tentative
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        ESP_LOGI(TAG, "Retry to connect...");
    }

    // Une adresse IP valide est attribuée → connexion OK
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        ESP_LOGI(TAG, "WiFi connected!");
    }
}

//Initialisation du Wi-Fi en mode station (STA)
void wifi_init_sta(void)
{

    wifi_event_group = xEventGroupCreate();

    // Initialisation de la pile Wi-Fi / réseau
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Interface Wi-Fi en mode STA
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));


    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID, 
        &event_handler,
        NULL,
        &instance_any_id
    ));

    // Gestionnaire spécifique lorsque l'ESP32 reçoit une IP
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &event_handler,
        NULL,
        &instance_got_ip
    ));

    // Paramètres du réseau (SSID et mot de passe)
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    // Mode station + application de la configuration
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    // Démarrage final
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi init done");
}
