#include <libusb-1.0/libusb.h>
#include <iostream>
#include "USBIO.h"

n_USBIO::USBIO::USBIO(libusb_device_handle* handle,int interface):
    m_interface(interface),
    m_closed(false)
{ m_handle = handle; }

n_USBIO::USBIO::~USBIO(){
    if(m_handle != NULL) delete m_handle;
}

int n_USBIO::USBIO::init(){
    std::cout << "Default init method. Override me!" << std::endl;
    return 0;
}

bool n_USBIO::USBIO::is_open(){
    return !m_closed;
}

/*
 * bytes must be 16 elements long
 * It does not matter what data is in bytes, as it will all be overwritten
 * size should be the size of each inner array (not counting NULL)
 */
int n_USBIO::USBIO::read(unsigned char** bytes,ssize_t size){
    int i=0;
    for(int e=0;e!=15;e++){
        int err = this->read(bytes[i],size,e);
        if(err) return err;
        i++;
    }
    return 0;
}

// Read size bytes into array bytes from endpoint point
int n_USBIO::USBIO::read(unsigned char* bytes,ssize_t size,int point){
    int actual;
    int success = libusb_bulk_transfer(m_handle,
                    (point|LIBUSB_ENDPOINT_IN),
                     bytes,size,&actual,0);
    return success;
}

/*
 * When calling this function, bytes should be an array of both uchar arrays and NULL
 * bytes must be 16 elements long
 * bytes = {
 *  {0xAE58800},
 *  NULL,
 *  {0x21,0x555},
 *  {0x6755,0x0},
 *  NULL,
 *  etc...
 * }
 * size should be the size of each inner array (not counting NULL)
 */
int n_USBIO::USBIO::write(unsigned char** bytes,ssize_t size){
    int i=0;
    for(int e=0; e!=15; e++){
        int err = this->write(bytes[i],size,e);
        if(err) return err;
        i++;
    }
    return 0;
}

// Write size bytes from array bytes to endpoint point
int n_USBIO::USBIO::write(unsigned char* bytes,ssize_t size,int point){
    // Don't do anything if bytes has nothing in it
    if(bytes == NULL) return 0;

    int actual;
    int success = libusb_bulk_transfer(m_handle,
            (point|LIBUSB_ENDPOINT_OUT),
             bytes,size,&actual,0);
    return success;
}

int n_USBIO::USBIO::close(){
    return libusb_release_interface(m_handle,m_interface);
}

// Wrapper functions for libusb_init and libusb_exit
// Really, these are actually pretty useless
int n_USBIO::init(){
    return libusb_init(NULL);
}

void n_USBIO::exit(){
    libusb_exit(NULL);
}

