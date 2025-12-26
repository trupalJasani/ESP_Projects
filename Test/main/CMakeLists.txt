idf_component_register(SRCS "main.c" 
                    INCLUDE_DIRS "."
                    REQUIRES led_strip
                    PRIV_REQUIRES esp_driver_gpio)