idf_component_register(SRCS "main.cpp" "lib/WiFi.cpp" "lib/Http.cpp" "lib/SSD1306.cpp" "lib/I2C.cpp"
                    REQUIRES driver esp_wifi nvs_flash esp_http_client
                    INCLUDE_DIRS "." "include")
