#ifndef MEATLOAF_H
#define MEATLOAF_H

#include <cstdint>
#include <cstring>

#include "bus.h"
#include "network.h"
#include "cassette.h"
#include "fnWiFi.h"

#include "../fuji/fujiHost.h"
#include "../fuji/fujiDisk.h"
#include "../fuji/fujiCmd.h"

#define MAX_HOSTS 8
#define MAX_DISK_DEVICES 8
#define MAX_NETWORK_DEVICES 4

// only in BASIC:
#define MAX_APPKEY_LEN 64

typedef struct
{
    char ssid[33];
    char password[64];
    char hostname[64];
    unsigned char localIP[4];
    unsigned char gateway[4];
    unsigned char netmask[4];
    unsigned char dnsIP[4];
    unsigned char macAddress[6];
    unsigned char bssid[6];
    char fn_version[15];
} AdapterConfig;

enum appkey_mode : int8_t
{
    APPKEYMODE_INVALID = -1,
    APPKEYMODE_READ = 0,
    APPKEYMODE_WRITE,
    APPKEYMODE_READ_256
};

struct appkey
{
    uint16_t creator = 0;
    uint8_t app = 0;
    uint8_t key = 0;
    appkey_mode mode = APPKEYMODE_INVALID;
    uint8_t reserved = 0;
} __attribute__((packed));

typedef struct
{
    char ssid[33];
    uint8_t rssi;
} scan_result_t;

typedef struct
{
    char ssid[MAX_SSID_LEN + 1];
    char password[MAX_PASSPHRASE_LEN + 1];
} net_config_t;

class iecMeatloaf : public iecDrive
{
private:
    systemBus *_bus;

    // fujiHost _fnHosts[MAX_HOSTS];

    // fujiDisk _fnDisks[MAX_DISK_DEVICES];

    // int _current_open_directory_slot = -1;

    iecDrive _bootDisk; // special disk drive just for configuration

    uint8_t bootMode = 0; // Boot mode 0 = CONFIG, 1 = MINI-BOOT

    uint8_t _countScannedSSIDs = 0;

    appkey _current_appkey;

    AdapterConfig cfg;

    std::string response;
    std::vector<uint8_t> responseV;
    bool is_raw_command;

    void process_raw_commands();
    void process_basic_commands();
    vector<string> tokenize_basic_command(string command);

    bool validate_parameters_and_setup(uint8_t& maxlen, uint8_t& addtlopts);
    bool validate_directory_slot();
    std::string process_directory_entry(uint8_t maxlen, uint8_t addtlopts);
    void format_and_set_response(const std::string& entry);

protected:
    // helper functions
    void net_store_ssid(std::string ssid, std::string password);

    // 0xFF
    void reset_device();

    // 0xFE
    net_config_t net_get_ssid();
    void net_get_ssid_basic();
    void net_get_ssid_raw();

    // 0xFD
    void net_scan_networks();
    void net_scan_networks_basic();
    void net_scan_networks_raw();

    // 0xFC
    scan_result_t net_scan_result(int scan_num);
    void net_scan_result_basic();
    void net_scan_result_raw();
    
    // 0xFB
    void net_set_ssid(bool store, net_config_t& net_config);
    void net_set_ssid_basic(bool store = true);
    void net_set_ssid_raw(bool store = true);

    // 0xFA
    uint8_t net_get_wifi_status();
    void net_get_wifi_status_basic();
    void net_get_wifi_status_raw();
    
    // 0xF9
    bool mount_host(int hs);
    void mount_host_basic();
    void mount_host_raw();

    // 0xF8
    bool disk_image_mount(uint8_t ds, uint8_t mode);
    void disk_image_mount_basic();
    void disk_image_mount_raw();

    bool open_directory(uint8_t hs, std::string dirpath, std::string pattern); // 0xF7
    void open_directory_basic();
    void open_directory_raw();

    void read_directory_entry();   // 0xF6
    void close_directory();        // 0xF5
    void read_host_slots();        // 0xF4
    void write_host_slots();       // 0xF3
    void read_device_slots();      // 0xF2
    void write_device_slots();     // 0xF1
    
    void enable_udpstream();       // 0xF0
    void net_get_wifi_enabled();   // 0xEA
    
    void disk_image_umount();      // 0xE9
    
    void get_adapter_config();     // 0xE8
    
    void new_disk();               // 0xE7
    void unmount_host();           // 0xE6

    void get_directory_position(); // 0xE5
    void set_directory_position(); // 0xE4

    void set_hindex();             // 0xE3
    void set_device_filename();    // 0xE2
    void set_host_prefix();        // 0xE1
    void get_host_prefix();        // 0xE0
    
    void set_external_clock();     // 0xDF
    
    // 0xDE
    void write_app_key(std::vector<uint8_t>&& value);
    void write_app_key_basic();
    void write_app_key_raw();

    void read_app_key();           // 0xDD
    void open_app_key();           // 0xDC
    void close_app_key();          // 0xDB
    
    void get_device_filename();    // 0xDA
    void set_boot_config();        // 0xD9
    void copy_file();              // 0xD8
    void set_boot_mode();          // 0xD6

    // Commodore specific
    void local_ip();

    device_state_t process() override;

    void shutdown() override;

    int appkey_size = 64;
    std::map<int, int> mode_to_keysize = {
        {0, 64},
        {2, 256}
    };
    bool check_appkey_creator(bool check_is_write);
    bool check_sd_running();

    /**
     * @brief called to process command either at open or listen
     */
    void iec_command();

public:
    iecMeatloaf();
    ~iecMeatloaf();

    bool boot_config = true;

    bool status_wait_enabled = true;
    
    //iecNetwork *network();

    iecDrive *bootdisk();

    void insert_boot_device(uint8_t d);

    void setup(systemBus *bus);

    void image_rotate();
    int get_disk_id(int drive_slot);
    std::string get_host_prefix(int host_slot);

//    fujiHost *get_hosts(int i) { return &_fnHosts[i]; }
//    fujiDisk *get_disks(int i) { return &_fnDisks[i]; }

    void _populate_slots_from_config();
    void _populate_config_from_slots();

    void mount_all();              // 0xD7
};

extern iecMeatloaf Meatloaf;

#endif // MEATLOAF_H