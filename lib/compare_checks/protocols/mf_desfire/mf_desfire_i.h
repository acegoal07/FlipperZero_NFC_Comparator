#pragma once

#include <nfc/protocols/mf_desfire/mf_desfire.h>

#include "mf_desfire.h"
#include "../../nfc_comparator_compare_checks_i.h"

/**
 * @enum MFDesfireFields
 * @brief Enumeration of MF Desfire payment card fields for comparison
 */
typedef enum {
   MFDesfireFields_Applications,
   MFDesfireFields_HardwareVersion,
   MFDesfireFields_SoftwareVersion
} MFDesfireFields;
