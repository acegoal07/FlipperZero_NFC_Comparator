#pragma once

#include <furi.h>
#include <nfc_device.h>
#include <nfc/protocols/mf_classic/mf_classic.h>
#include <nfc/protocols/st25tb/st25tb.h>
#include <nfc/protocols/mf_ultralight/mf_ultralight.h>
#include <nfc/protocols/felica/felica.h>
#include <nfc/protocols/type_4_tag/type_4_tag.h>
#include <nfc/protocols/iso15693_3/iso15693_3.h>
#include <nfc/protocols/slix/slix.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @enum NfcCompareChecksType
 * @brief Type of NFC comparison being performed.
 */
typedef enum {
   NfcCompareChecksType_Shallow,
   NfcCompareChecksType_Deep,
   NfcCompareChecksType_Undefined
} NfcCompareChecksType;

/**
 * @enum NfcCompareChecksDiffUnit
 * @brief The data type which is being compared in depth
 */
typedef enum {
   NfcCompareChecksComparedDataType_Blocks,
   NfcCompareChecksComparedDataType_Pages,
   NfcCompareChecksComparedDataType_Bytes,
   NfcCompareChecksComparedDataType_Unkown
} NfcCompareChecksDiffUnit;

/**
 * @struct NfcComparatorCompareChecks
 * @brief Structure holding the results of NFC comparison checks.
 */
typedef struct {
   NfcCompareChecksType compare_type;
   FuriString* nfc_card_path;
   struct {
      bool uid;
      bool uid_length;
      bool protocol;
      bool nfc_data;
   } results;
   struct {
      NfcCompareChecksDiffUnit unit;
      uint16_t indices[2048];
      uint16_t count;
      uint16_t total;
   } diff;
} NfcComparatorCompareChecks;

/**
 * @brief Allocate a new NfcComparatorCompareChecks structure.
 * @return Pointer to the allocated structure, or NULL on failure.
 */
NfcComparatorCompareChecks* nfc_comparator_compare_checks_alloc(void);

/**
 * @brief Free a NfcComparatorCompareChecks structure.
 * @param checks Pointer to the structure to free.
 */
void nfc_comparator_compare_checks_free(NfcComparatorCompareChecks* checks);

/**
 * @brief Copy checks data from one compare checks to another
 * @param destination Where the data should be copied to
 * @param data The data that should be copied to the destination
 */
void nfc_comparator_compare_checks_copy(
   NfcComparatorCompareChecks* destination,
   NfcComparatorCompareChecks* data);

/**
 * @brief Reset the fields of a NfcComparatorCompareChecks structure.
 * @param checks Pointer to the structure to reset.
 */
void nfc_comparator_compare_checks_reset(NfcComparatorCompareChecks* checks);

/**
 * @brief Compare two NFC cards and update the checks structure.
 * @param checks Pointer to the checks structure.
 * @param card1 Pointer to the first NFC card.
 * @param card2 Pointer to the second NFC card.
 * @param check_data Whether to compare NFC data.
 */
void nfc_comparator_compare_checks_compare_cards(
   NfcComparatorCompareChecks* checks,
   const struct NfcDevice* card1,
   const struct NfcDevice* card2);

#ifdef __cplusplus
}
#endif
