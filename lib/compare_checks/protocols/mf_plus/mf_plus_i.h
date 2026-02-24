#pragma once

#include <nfc/protocols/mf_plus/mf_plus.h>

#include "mf_plus.h"
#include "../../nfc_comparator_compare_checks_i.h"

/**
 * @enum MFPlusFields
 * @brief Enumeration of MF Plus payment card fields for comparison
 */
typedef enum {
   MFPlusFields_HardwareVersion,
   MFPlusFields_SecurityLevel,
   MFPlusFields_Size,
   MFPlusFields_SoftwareVersion,
   MFPlusFields_Type
} MFPlusFields;
