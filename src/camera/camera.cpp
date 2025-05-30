#include "camera/camera.hpp"
#include <thread>
#include <chrono>
#include <cstring>
#include <iostream>
#include <gphoto2/gphoto2-camera.h>

CameraGPhoto2::CameraGPhoto2(Camera *camera, GPContext *context) : camera(camera), context(context) {}

CameraGPhoto2::CameraGPhoto2() : camera(nullptr), context(nullptr), name(nullptr), port(nullptr)
{
    this->context = gp_context_new();

    CameraList *list;

    int result = gp_list_new(&list);
    if (result < GP_OK)
    {
        std::cout << "Failed to create camera list: " << gp_result_as_string(result) << std::endl;
        return;
    }

    result = gp_camera_autodetect(list, this->context);
    if (result < GP_OK)
    {
        std::cout << "Failed to autodetect cameras: " << gp_result_as_string(result) << std::endl;
        return;
    }

    size_t count = gp_list_count(list);
    std::cout << "Number of cameras detected: " << count << std::endl;
    if (count == 0)
    {
        std::cout << "No cameras found." << std::endl;
        gp_list_free(list);
        return;
    }

    std::cout << "Detected cameras:" << std::endl;
    for (size_t i = 0; i < count; i++)
    {
        const char *name, *port;
        gp_list_get_name(list, i, &name);
        gp_list_get_value(list, i, &port);
        std::cout << name << " on " << port << std::endl;
    }

    // Using the first found camera. Needs to be changed
    result = gp_camera_new(&this->camera);
    if (result < GP_OK)
    {
        std::cout << "Failed to create camera object: " << gp_result_as_string(result) << std::endl;
        gp_list_free(list);
        gp_context_unref(this->context);
        return;
    }

    gp_list_get_name(list, 0, &this->name);

    GPPortInfoList *portinfolist = nullptr;
    gp_port_info_list_new(&portinfolist);
    gp_port_info_list_load(portinfolist);

    gp_list_get_value(list, 0, &port);
    int port_index = gp_port_info_list_lookup_path(portinfolist, port);
    GPPortInfo portinfo;
    gp_port_info_list_get_info(portinfolist, port_index, &portinfo);
    gp_camera_set_port_info(this->camera, portinfo);

    // INIT
    result = gp_camera_init(this->camera, this->context);
    if (result < GP_OK)
    {
        std::cout << "Failed to initialize camera: " << gp_result_as_string(result) << std::endl;
        gp_camera_free(this->camera);
        gp_list_free(list);
        gp_port_info_list_free(portinfolist);
        gp_context_unref(this->context);
        return;
    }
}

CameraGPhoto2::~CameraGPhoto2()
{
    if (camera)
    {
        gp_camera_free(camera);
    }
    if (context)
    {
        gp_context_unref(context);
    }
}

const char *CameraGPhoto2::get_name() const
{
    return this->name;
}

const char *CameraGPhoto2::get_port() const
{
    return this->port;
}

const char *CameraGPhoto2::get_iso() const
{
    static char iso_buffer[128];
    CameraWidget *rootconfig = nullptr, *child = nullptr;
    if (gp_camera_get_config(camera, &rootconfig, context) < GP_OK)
    {
        return "Unknown ISO";
    }
    if (gp_widget_get_child_by_name(rootconfig, "iso", &child) < GP_OK)
    {
        gp_widget_free(rootconfig);
        return "ISO Not Set";
    }
    CameraWidgetType type;
    gp_widget_get_type(child, &type);
    if (type == GP_WIDGET_TEXT || type == GP_WIDGET_MENU || type == GP_WIDGET_RADIO)
    {
        const char *iso_value = nullptr;
        gp_widget_get_value(child, &iso_value);
        if (iso_value)
        {
            strncpy(iso_buffer, iso_value, sizeof(iso_buffer) - 1);
            iso_buffer[sizeof(iso_buffer) - 1] = '\0';
            gp_widget_free(rootconfig);
            return iso_buffer;
        }
    }
    gp_widget_free(rootconfig);
    return "ISO Not Available";
}

const char *CameraGPhoto2::get_shutter_speed() const
{
    static char shutter_buffer[128];
    CameraWidget *rootconfig = nullptr, *child = nullptr;
    if (gp_camera_get_config(camera, &rootconfig, context) < GP_OK)
    {
        return "Unknown Shutter Speed";
    }
    if (gp_widget_get_child_by_name(rootconfig, "shutterspeed", &child) < GP_OK)
    {
        gp_widget_free(rootconfig);
        return "Shutter Speed Not Set";
    }
    CameraWidgetType type;
    gp_widget_get_type(child, &type);
    if (type == GP_WIDGET_TEXT || type == GP_WIDGET_MENU || type == GP_WIDGET_RADIO)
    {
        const char *shutter_value = nullptr;
        gp_widget_get_value(child, &shutter_value);
        if (shutter_value)
        {
            strncpy(shutter_buffer, shutter_value, sizeof(shutter_buffer) - 1);
            shutter_buffer[sizeof(shutter_buffer) - 1] = '\0';
            gp_widget_free(rootconfig);
            return shutter_buffer;
        }
    }
    gp_widget_free(rootconfig);
    return "Shutter Speed Not Available";
}

size_t CameraGPhoto2::get_bulb_speed() const
{
    return bulb_speed; // Return the stored bulb speed
}

void CameraGPhoto2::set_iso(const char *iso)
{
    CameraWidget *rootconfig = nullptr, *child = nullptr;
    if (gp_camera_get_config(camera, &rootconfig, context) < GP_OK)
    {
        return;
    }
    if (gp_widget_get_child_by_name(rootconfig, "iso", &child) < GP_OK)
    {
        gp_widget_free(rootconfig);
        return;
    }
    gp_widget_set_value(child, iso);
    gp_camera_set_config(camera, rootconfig, context);
    gp_widget_free(rootconfig);
}

void CameraGPhoto2::set_shutter_speed(const char *shutter_type)
{
    CameraWidget *rootconfig = nullptr, *child = nullptr;
    if (gp_camera_get_config(camera, &rootconfig, context) < GP_OK)
    {
        return;
    }
    if (gp_widget_get_child_by_name(rootconfig, "shutterspeed", &child) < GP_OK)
    {
        gp_widget_free(rootconfig);
        return;
    }
    gp_widget_set_value(child, shutter_type);
    gp_camera_set_config(camera, rootconfig, context);
    gp_widget_free(rootconfig);
}

void CameraGPhoto2::set_bulb_speed(size_t speed)
{
    bulb_speed = speed; // Store the bulb speed
    // Note: Actual implementation to set bulb speed may vary based on camera capabilities
}

int CameraGPhoto2::SHOOT(const char *filename)
{
    CameraFilePath camera_file_path;
    bool file_ready = false;

    if (strcmp(this->get_shutter_speed(), "bulb") == 0)
    {
        // camerawidget
        CameraWidget *rootconfig = nullptr, *child = nullptr;
        int ret = gp_camera_get_config(camera, &rootconfig, context);
        if (ret < GP_OK)
        {
            std::cerr << "Failed to get camera config: " << gp_result_as_string(ret) << std::endl;
            return ret;
        }

        // eosremoterelease=Immediate
        ret = gp_widget_get_child_by_name(rootconfig, "eosremoterelease", &child);
        if (ret < GP_OK)
        {
            std::cerr << "Config eosremoterelease not found." << std::endl;
            gp_widget_free(rootconfig);
            return ret;
        }
        ret = gp_widget_set_value(child, "Immediate");
        if (ret < GP_OK)
        {
            std::cerr << "Failed to set eosremoterelease=Immediate" << std::endl;
            gp_widget_free(rootconfig);
            return ret;
        }

        // setting config
        ret = gp_camera_set_config(camera, rootconfig, context);
        if (ret < GP_OK)
        {
            std::cerr << "Failed to set camera config: " << gp_result_as_string(ret) << std::endl;
            gp_widget_free(rootconfig);
            return ret;
        }
        gp_widget_free(rootconfig);

        // hjonk shoo mimimimi
        std::this_thread::sleep_for(std::chrono::seconds(bulb_speed));

        // widget 2 electric boogaloo
        ret = gp_camera_get_config(camera, &rootconfig, context);
        if (ret < GP_OK)
        {
            std::cerr << "Failed to get camera config: " << gp_result_as_string(ret) << std::endl;
            return ret;
        }

        // eosremoterelease=Release
        ret = gp_widget_get_child_by_name(rootconfig, "eosremoterelease", &child);
        if (ret < GP_OK)
        {
            std::cerr << "Config eosremoterelease not found." << std::endl;
            gp_widget_free(rootconfig);
            return ret;
        }
        ret = gp_widget_set_value(child, "Release Full");
        if (ret < GP_OK)
        {
            std::cerr << "Failed to set eosremoterelease=Release Full" << std::endl;
            gp_widget_free(rootconfig);
            return ret;
        }

        // setting config once again
        ret = gp_camera_set_config(camera, rootconfig, context);
        if (ret < GP_OK)
        {
            std::cerr << "Failed to set camera config: " << gp_result_as_string(ret) << std::endl;
            gp_widget_free(rootconfig);
            return ret;
        }

        // free stuff
        gp_widget_free(rootconfig);
        //std::cout << "Exposure ended (Release Full)" << std::endl;

        // Wait for a file added event
        CameraEventType evtype;
        void *evdata = nullptr;
        int timeout_ms = 5000; // Wait up to 5 seconds

        while (true)
        {
            int ret = gp_camera_wait_for_event(camera, timeout_ms, &evtype, &evdata, context);
            if (ret < GP_OK)
            {
                std::cerr << "Error waiting for event: " << gp_result_as_string(ret) << std::endl;
                break;
            }
            if (evtype == GP_EVENT_FILE_ADDED)
            {
                CameraFilePath *fp = (CameraFilePath *)evdata;
                camera_file_path = *fp;
                file_ready = true;
                break;
            }
            else if (evtype == GP_EVENT_TIMEOUT)
            {
                std::cerr << "Timeout waiting for file event." << std::endl;
                break;
            }
            if (evdata)
            {
                evdata = nullptr;
            }
        }
    }
    else
    {

        int ret = gp_camera_capture(camera, GP_CAPTURE_IMAGE, &camera_file_path, context);
        if (ret < GP_OK)
        {
            //std::cerr << "Failed to capture image: " << gp_result_as_string(ret) << std::endl;
            return ret;
        }
        std::cout << "Captured image: " << camera_file_path.folder << camera_file_path.name << std::endl;
        file_ready = true;
    }

    // Shared file saving logic
    if (file_ready)
    {
        CameraFile *file;
        gp_file_new(&file);
        int ret = gp_camera_file_get(camera, camera_file_path.folder, camera_file_path.name, GP_FILE_TYPE_NORMAL, file, context);
        if (ret == GP_OK)
        {
            gp_file_save(file, filename);
            std::cout << "Downloaded to " << filename << std::endl;
        }
        else
        {
            std::cerr << "Failed to get file: " << gp_result_as_string(ret) << std::endl;
        }
        gp_file_free(file);
    }

    return GP_OK;
}