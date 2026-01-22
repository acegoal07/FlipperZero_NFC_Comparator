#pragma once

#include <furi.h>
#include <nfc_device.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @enum NfcCompareChecksType
 * @brief Type of NFC comparison being performed.
 */
typedef enum {
   NfcCompareChecksType_Digital, /**< Comparison between two digital NFC cards */
   NfcCompareChecksType_Physical, /**< Comparison between a physical and a digital NFC card */
   NfcCompareChecksType_Undefined /**< Comparison type is undefined */
} NfcCompareChecksType;

/**
 * @struct NfcComparatorCompareChecks
 * @brief Structure holding the results of NFC comparison checks.
 */
typedef struct {
   FuriString* nfc_card_path;
   bool uid;
   bool uid_length;
   bool protocol;
   bool nfc_data;
   NfcCompareChecksType type;
   uint16_t diff_blocks[64];
   uint16_t diff_count;
   uint16_t total_blocks; 
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
 * @brief Reset the fields of a NfcComparatorCompareChecks structure.
 * @param checks Pointer to the structure to reset.
 */
void nfc_comparator_compare_checks_reset(NfcComparatorCompareChecks* checks);

/**
 * @brief Get the comparison type.
 * @param checks Pointer to the structure.
 * @return The comparison type.
 */
NfcCompareChecksType
   nfc_comparator_compare_checks_get_type(const NfcComparatorCompareChecks* checks);

/**
 * @brief Set the comparison type.
 * @param checks Pointer to the structure.
 * @param type The type to set.
 */
void nfc_comparator_compare_checks_set_type(
   NfcComparatorCompareChecks* checks,
   NfcCompareChecksType type);

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
   const struct NfcDevice* card2,
   bool check_data);

#ifdef __cplusplus
}
#endif
