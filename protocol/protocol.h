
enum skim_client_id_t
{
    skim_client_id_nak,
};

enum skim_host_id_t
{
    skim_host_id_version,
    skim_host_id_gain,
    skim_host_id_offset
};

struct skim_client_header_t
{
    enum skim_client_id_t   id;
    uint8_t                 length;
   
};

struct skim_host_header_t
{
    enum skim_host_id_t     id;
    uint8_t                 length;
   
};

struct skim_client_version_t
{
    struct skim_client_header_t    hdr;
    uint8_t                 major;
    uint8_t                 minor;
    uint16_t                crc;
};

struct skim_host_version_t
{
    struct skim_host_header_t    hdr;
    uint16_t                crc;
};

struct skim_host_gain_offset_t
{
    struct skim_host_header_t    hdr;
    uint8_t                 channel;
    uint24_t                gain;
    uint24_t                offset;
    uint16_t                crc;
};

enum skim_client_nak_code_t
{
    skim_nak_unknown_command,
    skim_nak_invalid_argument
};

struct skim_client_nak_t
{
    struct skim_client_header_t                hdr;
    enum skim_client_nak_code_t         code;
    uint16_t                            crc;
};

union skim_host_t
{
    struct skim_host_header_t                hdr;
    struct skim_host_version_t          version;
    struct skim_host_gain_offset_t      gain_offset;
};

union skim_client_t
{
    struct skim_client_header_t                hdr;
    struct skim_client_nak_t            nak;
    struct skim_client_version_t        version;
};


