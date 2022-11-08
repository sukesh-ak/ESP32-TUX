/*
MIT License

Copyright (c) 2022 Sukesh Ashok Kumar

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/* 
    LV_FS Drive is named as F (ie Flash) in lv_conf.h
    Loaded SPIFF partition is mapped to drive letter F
    This enables to use the drive as F:/readme.txt
*/
#include <misc/lv_fs.h>

#define LV_README_TXT "F:/readme.txt"

static esp_err_t lv_print_readme_txt(void)
{

    if (lv_fs_is_ready('F'))
        ESP_LOGE(TAG, "F Drive is ready");
    else
        ESP_LOGE(TAG, "F Drive is NOT ready");

    lv_fs_file_t f;
    lv_fs_res_t res;
    res = lv_fs_open(&f, LV_README_TXT, LV_FS_MODE_RD);
    if(res != LV_FS_RES_OK) {
        ESP_LOGE(TAG, "Failed to open %s, %d",LV_README_TXT, res);
        return ESP_FAIL;
    }

    uint32_t read_num;
    uint8_t buf[100];
    res = lv_fs_read(&f, buf, 8, &read_num);
    if(res != LV_FS_RES_OK || read_num != 8) {
        ESP_LOGE(TAG, "Failed to read from %s", LV_README_TXT);
        return ESP_FAIL;
    }
    
    // Display the read contents from the file
    ESP_LOGI(TAG, "Read from %s : %s",LV_README_TXT, buf);
    lv_fs_close(&f);
    return ESP_OK;
}