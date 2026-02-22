#pragma once

#include <nfc/protocols/emv/emv.h>

#include "emv.h"
#include "../../nfc_comparator_compare_checks_i.h"

/**
 * @enum EmvFields
 * @brief Enumeration of EMV payment card fields for comparison
 */
typedef enum {
   EmvField_AID,
   EmvField_ApplicationLabel,
   EmvField_ApplicationName,
   EmvField_CardNumber,
   EmvField_CardHolder,
   EmvField_CountryCode,
   EmvField_CurrencyCode,
   EmvField_ExpirationDate
} EmvFields;
