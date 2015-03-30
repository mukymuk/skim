
struct skim_rsp_version_t
{
    uint8_t major;
    uint8_t minor;
};

struct skim_rsp_gain_offset_t
{
    uint8_t     channel;
    uint24_t    gain;
    uint24_t    offset;
};

enum skim_rsp_nak_code_t
{
    skim_rsp_nak_unknown_command,
    skim_rsp_nak_invalid_argument
};

struct skim_rsp_nak_t
{
    enum skim_rsp_nak_code_t code;
};

union skim_rsp_t
{
    struct skim_rsp_version_t       version;
    struct skim_rsp_nak_t           nak;
    struct skim_rsp_gain_offset_t   gain_offset;
};


union skim_cmd_t
{
};


enum skim_cmd_id_t
{
    skim_cmd_id_version,
    skim_cmd_id_gain,
    skim_cmd_id_offset
};

enum skim_rsp_id_t
{
    skim_rsp_id_nak,
    skim_rsp_id_gain,
    skim_rsp_id_offset
};

