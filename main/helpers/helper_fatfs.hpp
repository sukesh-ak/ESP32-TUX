#include "esp_vfs.h"
#include "esp_vfs_fat.h"

// Handle of the wear levelling library instance
static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

esp_err_t init_fatfs()
{
    ESP_LOGI(TAG, "Mounting FAT filesystem");
    const esp_vfs_fat_mount_config_t mount_config = {
            .max_files = 4,
            .format_if_mount_failed = false,
            .allocation_unit_size = CONFIG_WL_SECTOR_SIZE
    };
    esp_err_t err;
    err = esp_vfs_fat_spiflash_mount_rw_wl("/fatfs", "storage", &mount_config, &s_wl_handle);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return;
    }
}

static void print_readme_txt(void)
{
    // Not error but to highlight in output
    ESP_LOGE(TAG, "Reading readme.txt from FATFS");

    // Open for reading hello.txt
    FILE* f = fopen("/fatfs/readme.txt", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open /fatfs/readme.txt");
        return;
    }

    char buf[100];
    memset(buf, 0, sizeof(buf));
    fread(buf, 1, sizeof(buf), f);
    fclose(f);

    // Display the read contents from the file
    ESP_LOGI(TAG, "Read from /fatfs/readme.txt : %s", buf);
}
