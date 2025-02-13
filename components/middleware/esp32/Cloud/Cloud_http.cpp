#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_crt_bundle.h"
#include "Cloud_Private.hpp"
#include "Cloud.hpp"

/// @brief Performs an HTTP GET request with a bearer authentication token
/// in the header.
/// @param path Target route (the host is configured by the macro HTTP_Host)
/// @param token Access token
/// @param status_code Unsigned short pointer, this value will store the http status
/// code
/// @return NULL on error, or a new-allocated string containing the result
char *http_get_bearer(const char *path, const char *token,
                    unsigned short *status_code)
{
    esp_err_t err = 0;
    esp_http_client_config_t conf;
    esp_http_client_handle_t http_handle;
    char *output_buffer = new char[HTTP_BUFFER_SIZE] {'\0'};
    int content_length = 0;
    bool error = false;
    int data_read = 0;
    char bearer_header[strlen(token) + 8] = {'\0'};

    memset(&conf, 0, sizeof(conf));
    conf.method = HTTP_METHOD_GET;
    conf.skip_cert_common_name_check = HTTP_Skip_Name_Check;
    conf.transport_type = (HTTP_Use_TLS == true) ? HTTP_TRANSPORT_OVER_SSL : HTTP_TRANSPORT_OVER_TCP;
    conf.event_handler = NULL;
    conf.port = HTTP_Port;
    conf.host = HTTP_Host;
    conf.path = path;
    conf.crt_bundle_attach = (HTTP_Use_TLS == true) ? esp_crt_bundle_attach : NULL;
    strcpy(bearer_header, "Bearer: ");
    strcat(bearer_header, token);
    http_handle = esp_http_client_init(&conf);
    err = esp_http_client_set_header(http_handle, "Authorization", bearer_header);
    err = esp_http_client_open(http_handle, 0);
    if (err != ESP_OK) {
        error = true;
    } else {
        content_length = esp_http_client_fetch_headers(http_handle);
        if (content_length < 0) {
            error = true;
        } else {
            data_read = esp_http_client_read_response(http_handle, output_buffer, HTTP_BUFFER_SIZE);
            if (data_read < 0)
                error = true;
        }
    }
    if (error == true)
        ESP_LOGE(Cloud_LogTag, "HTTP Request failed with error code %d: %s", error, esp_err_to_name(error));
    if ((*status_code = esp_http_client_get_status_code(http_handle)) >= 400)
        error = true;
    esp_http_client_close(http_handle);
    esp_http_client_cleanup(http_handle);
    if (error == true) {
        delete output_buffer;
        return (NULL);
    }
    return (output_buffer);
}
