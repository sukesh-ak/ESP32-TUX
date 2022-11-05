#include "esp_spiffs.h"

esp_err_t init_spiff()
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = false
    };

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }        
    return ESP_OK;

    // esp_vfs_spiffs_unregister(NULL);
    // ESP_LOGI(TAG, "SPIFFS unmounted");    
}

static void print_readme_txt(void)
{
    // Not error but to highlight in output
    ESP_LOGE(TAG, "Reading readme.txt from SPIFFS");

    // Open for reading hello.txt
    FILE* f = fopen("/spiffs/readme.txt", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open /spiffs/readme.txt");
        return;
    }

    char buf[100];
    memset(buf, 0, sizeof(buf));
    fread(buf, 1, sizeof(buf), f);
    fclose(f);

    // Display the read contents from the file
    ESP_LOGI(TAG, "Read from /spiffs/readme.txt : %s", buf);
}