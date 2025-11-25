idf_component_register(SRCS "escape_room_main.c"
    INCLUDE_DIRS "."
    REQUIRES led buzzer push_button pir_sensor wifi esp_http_client)

