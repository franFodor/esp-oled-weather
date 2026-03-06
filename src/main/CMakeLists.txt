idf_component_register(SRCS "main.cpp" "network.cpp"
                    REQUIRES esp_wifi nvs_flash esp_http_client
                    INCLUDE_DIRS ".")
