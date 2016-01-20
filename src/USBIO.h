#include <libusb-1.0/libusb.h>

/*
// Forward declaration
static bool is_type_same(libusb_class_code,libusb_device*);
static int try_open_handle(libusb_device**,int,libusb_device_handle*,int);
*/

namespace n_USBIO{
    class USBIO {
        public:
            USBIO(libusb_device_handle*,int);
            ~USBIO();
            virtual int init();
            bool is_open();
            int read(unsigned char**,ssize_t);
            int read(unsigned char*,ssize_t,int);
            int write(unsigned char**,ssize_t);
            int write(unsigned char*,ssize_t,int);
            int close();
        private:
            libusb_device_handle* m_handle;
            bool m_closed;
            int m_interface;
    };

    int init();
    void exit();
};

// All of the opening shit can't be in n_USBIO because it won't compile
// Don't ask me why, I have no idea.

static bool is_type_same(libusb_class_code type, libusb_device* device){
    libusb_device_descriptor* desc;
    libusb_get_device_descriptor(device,desc);
    return type == desc->bDeviceClass;
}

// Seperated into a seperate function to make the code in open and open_type more readable.
static int try_open_handle(libusb_device** list,int count,libusb_device_handle* handle,int idx){
    if(count < 0){
        std::cerr << "Error: The device list has no devices!" << std::endl;
        return 1;
    }

    int r = libusb_open(list[idx],&handle);
    if(r){//libusb_open(list[idx],&handle)){
        std::cerr << "An error occurred while attempting to create a device handle!" << std::endl;
        std::cerr << libusb_error_name(r) << std::endl;
        return 1;
    }

    if(handle == NULL){
        std::cerr << "OH GOD! WHY IS HANDLE NULL?!" << std::endl;
        return 1;
    }
}

template<class T=n_USBIO::USBIO>
n_USBIO::USBIO* usbopen(libusb_class_code type,int interface,bool detach_kernel=true){
    if(n_USBIO::init()){
        std::cerr << "An error occurred during initialization!" << std::endl;
        return NULL;
    }
    libusb_device** list = NULL;
    libusb_device_handle* handle = NULL;

    ssize_t count = libusb_get_device_list(NULL,&list);

    for(ssize_t i=0;i<count;i++){
        if(is_type_same(type,list[i]))
            if(!try_open_handle(list,count,handle,i))
                break;
    }

    libusb_free_device_list(list,1);

    if(handle == NULL)
        return NULL;

    if(libusb_kernel_driver_active(handle,interface) == 1){
        if(detach_kernel){
            if(libusb_detach_kernel_driver(handle,interface) != 0){
                std::cerr << "Unable to detach Kernel Driver. Please contact a programmer." << std::endl;
                return NULL;
            }
        }else{
            std::cerr << "Kernel Driver attached. Cannot claim inteface. Try setting detach_kernel to true." << std::endl;
            return NULL;
        }
    }

    if(libusb_claim_interface(handle,interface)){
        std::cerr << "An error occurred when trying to claim interface." << std::endl;
        return NULL;
    }

    n_USBIO::USBIO* usb = new T(handle,interface);
    if(usb->init()) return NULL;
    return usb;
}


