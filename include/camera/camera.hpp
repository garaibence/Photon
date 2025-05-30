#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <string>
#include <gphoto2/gphoto2.h>

class RootCamera
{
public:
    virtual ~RootCamera() = default;

    virtual const char *get_name() const = 0;
    virtual const char *get_port() const = 0;
    virtual const char *get_iso() const = 0;
    virtual const char *get_shutter_speed() const = 0;
    virtual size_t get_bulb_speed() const = 0;

    virtual void set_iso(const char *iso) = 0;
    virtual void set_shutter_speed(const char *shutterType) = 0;
    virtual void set_bulb_speed(size_t speed) = 0;

    virtual int SHOOT(const char *filename) = 0;
};

class CameraGPhoto2 : public RootCamera
{
private:
    Camera *camera;             // Pointer to the gphoto2 camera object
    GPContext *context;         // Pointer to the gphoto2 context for operations
    size_t bulb_speed;           // Shutter speed for bulb operation
    const char *name, *port;    // Useful camera data

public:
    CameraGPhoto2(Camera *camera, GPContext *context);
    CameraGPhoto2();
    ~CameraGPhoto2() override;

    const char *get_name() const override;
    const char *get_port() const override;
    const char *get_iso() const override;
    const char *get_shutter_speed() const override;
    size_t get_bulb_speed() const override;

    void set_iso(const char *iso) override;
    void set_shutter_speed(const char *shutter_type) override;
    void set_bulb_speed(size_t speed) override;

    int SHOOT(const char *filename) override;
};

#endif // CAMERA_HPP