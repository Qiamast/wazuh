/**
 * @file fimCommonDefs.h
 * @brief Common definitions for FIM
 * @date 2021-09-06
 *
 * @copyright Copyright (C) 2015-2021 Wazuh, Inc.
 */

#ifndef DB_COMMONDEFS_H
#define DB_COMMONDEFS_H
#include "logging_helper.h"

#define FIMBD_FILE_TABLE_NAME "file_entry"
#define FILE_PRIMARY_KEY "path"

typedef enum FIMDBErrorCodes
{
    FIMDB_OK = 0,
    FIMDB_ERR = -1,
    FIMDB_FULL = -2
} FIMDBErrorCodes;

typedef void((*fim_sync_callback_t)(const char *tag, const char* buffer));
typedef void((*logging_callback_t)(const modules_log_level_t level, const char* log));

#endif // DB_STATEMENT_H