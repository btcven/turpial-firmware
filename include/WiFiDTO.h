/**
 * @file WiFiDTO.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1.1
 * @date 2019-11-02
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include "Serializable.h"
#include <iostream>
#include <hal/hardware.h>

/**
 * @brief This class represent the data that need to travel throught:
 *          1. System storage
 *          2. Sockets
 *          3. Serial communication
 * 
 */
typedef struct wifi_dto_config_t {
    int apChannel;
    int apMaxConn;
    int WAP_enabled;    // Default value
    int  WST_enabled;   // Default value
    int isOpen;
    char* apSSID;
    char* apPassword;
    
   
} wifi_dto_config_t;

/**
 * @brief 
 * 
 */
class WiFiDTO : public Serializable {

public:
    /**
     * @brief Construct a new WiFi D T O object
     * 
     * @param settings 
     */
    WiFiDTO(wifi_dto_config_t& settings);

    /**
     * @brief Construct a new WiFi D T O object
     * 
     */
    WiFiDTO();

    /**
     * @brief 
     * 
     * @return size_t 
     */
    virtual size_t serialize_size() const;

    /**
     * @brief 
     * 
     * @param dataOut 
     */
    virtual void serialize(char* dataOut) const;

    /**
     * @brief 
     * 
     * @param dataIn 
     */
    virtual void deserialize(const char* dataIn);

    /**
     * @brief Set the Data object
     * 
     * @param data 
     */
    void setData(wifi_dto_config_t data);

    /**
     * @brief 
     * 
     */
    void printData(void);

private:
    //wifi_dto_config_t settings_;
    wifi_dto_config_t* ptrSettings_;
    wifi_dto_config_t** initPtrSettings_ = &ptrSettings_;
};