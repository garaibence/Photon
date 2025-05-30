extern "C"
{
#include <gphoto2/gphoto2.h>
}

#include <iostream>
#include "camera/camera.hpp"

int main(int argc, char const *argv[])
{
    // Create CameraGPhoto2 instance
    CameraGPhoto2 cam;
    if (cam.get_name() == nullptr)
    {
        return -1;
    }

    std::cout << "Camera name: " << cam.get_name() << std::endl;
    std::cout << "Camera port: " << cam.get_port() << std::endl;
    /*
    std::cout << "Current ISO: " << cam.get_iso() << std::endl;
    std::cout << "Current Shutter Speed: " << cam.get_shutter_speed() << std::endl;
    */


    std::vector<std::string> choices = cam.list_shutter_speed();
    std::cout << choices.size() << std::endl;
    std::cout << "Available shutter speeds:" << std::endl;

    for (std::string choice: choices)
    {
        std::cout << choice << std::endl;
    }

    choices = cam.list_iso();
    std::cout << choices.size() << std::endl;
    std::cout << "Available ISOs:" << std::endl;
    for (std::string choice: choices)
    {
        std::cout << choice << std::endl;
    }
    /*

    cam.set_iso("100");
    std::cout << "ISO set to: " << cam.get_iso() << std::endl;

    cam.set_shutter_speed("1/4000");
    std::cout << "Shooting Bias frames" << std::endl;

    for (size_t i = 0; i < 10; i++)
    {
        std::cout << "Shooting #" << (i + 1) << "..." << std::endl;
        char filename[64];
        snprintf(filename, sizeof(filename), "./testphotos/Bias/Bias_%04zu.cr2", i + 1);
        cam.SHOOT(filename);
    }

    cam.set_bulb_speed(1); // 1 seconds for bulb mode
    std::cout << "Bulb Speed set to: " << cam.get_bulb_speed() << " seconds" << std::endl;
    cam.set_shutter_speed("bulb");
    
    for (size_t i = 0; i < 10; i++)
    {
        std::cout << "Shooting #" << (i + 1) << "..." << std::endl;
        char filename[64];
        snprintf(filename, sizeof(filename), "./testphotos/Lights/Light_%04zu.cr2", i + 1);
        cam.SHOOT(filename);
    }

    */

    // Cleanup
    // gp_camera_exit(camera, context);
    // gp_camera_free(camera);
    // gp_list_free(list);
    // gp_port_info_list_free(portinfolist);
    // gp_context_unref(context);

    return 0;
}
