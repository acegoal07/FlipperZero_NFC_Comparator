#include "nfc_comparator_compare_checks.h"

NfcComparatorCompareChecks* nfc_comparator_compare_checks_alloc() {
   NfcComparatorCompareChecks* checks = malloc(sizeof(NfcComparatorCompareChecks));
   furi_assert(checks);

   checks->compare_type = NfcCompareChecksType_Undefined;
   checks->nfc_card_path = furi_string_alloc();
   checks->uid = false;
   checks->uid_length = false;
   checks->protocol = false;
   checks->nfc_data = false;
   checks->diff_unit = NfcCompareChecksComparedDataType_Unkown;
   memset(checks->diff_blocks, 0, sizeof(checks->diff_blocks));
   checks->diff_count = 0;
   checks->total_blocks = 0;

   return checks;
}

void nfc_comparator_compare_checks_free(NfcComparatorCompareChecks* checks) {
   furi_assert(checks);
   furi_string_free(checks->nfc_card_path);
   free(checks);
}

void nfc_comparator_compare_checks_copy(
   NfcComparatorCompareChecks* destination,
   NfcComparatorCompareChecks* data) {
   furi_assert(destination);
   furi_assert(data);
   destination->compare_type = data->compare_type;
   furi_string_reset(destination->nfc_card_path);
   furi_string_set(destination->nfc_card_path, data->nfc_card_path);
   destination->uid = data->uid;
   destination->uid_length = data->uid_length;
   destination->protocol = data->protocol;
   destination->nfc_data = data->nfc_data;
   destination->diff_unit = data->diff_unit;
   destination->total_blocks = data->total_blocks;
   destination->diff_count = data->diff_count;
   memcpy(destination->diff_blocks, data->diff_blocks, sizeof(destination->diff_blocks));
}

void nfc_comparator_compare_checks_reset(NfcComparatorCompareChecks* checks) {
   furi_assert(checks);
   checks->compare_type = NfcCompareChecksType_Undefined;
   furi_string_reset(checks->nfc_card_path);
   checks->uid = false;
   checks->uid_length = false;
   checks->protocol = false;
   checks->nfc_data = false;
   checks->diff_unit = NfcCompareChecksComparedDataType_Unkown;
   memset(checks->diff_blocks, 0, sizeof(checks->diff_blocks));
   checks->diff_count = 0;
   checks->total_blocks = 0;
}

void nfc_comparator_compare_checks_set_type(
   NfcComparatorCompareChecks* checks,
   NfcCompareChecksType type) {
   furi_assert(checks);
   furi_assert(type);
   checks->compare_type = type;
}

void nfc_comparator_compare_checks_compare_cards(
   NfcComparatorCompareChecks* checks,
   const struct NfcDevice* card1,
   const struct NfcDevice* card2) {
   furi_assert(checks);
   furi_assert(card1);
   furi_assert(card2);

   // Reset difference counter
   checks->diff_count = 0;
   checks->total_blocks = 0;

   // Compare protocols
   checks->protocol = nfc_device_get_protocol(card1) == nfc_device_get_protocol(card2);

   // Get UIDs and lengths
   size_t uid_len1 = 0, uid_len2 = 0;
   const uint8_t* uid1 = nfc_device_get_uid(card1, &uid_len1);
   const uint8_t* uid2 = nfc_device_get_uid(card2, &uid_len2);

   // Compare UID length
   checks->uid_length = (uid_len1 == uid_len2);

   // Compare UID bytes if lengths match
   if(checks->uid_length) {
      checks->uid = (memcmp(uid1, uid2, uid_len1) == 0);
   } else {
      checks->uid = false;
   }

   // compare NFC data
   if(checks->compare_type == NfcCompareChecksType_Deep) {
      checks->nfc_data = nfc_device_is_equal(card1, card2);

      // COMPARE LOGIC
      if(!checks->nfc_data && checks->protocol) {
         switch(nfc_device_get_protocol(card1)) {
         // Felica
         case NfcProtocolFelica: {
            const FelicaData* card1_data = nfc_device_get_data(card1, NfcProtocolFelica);
            const uint8_t card1_block_count = card1_data->blocks_total;
            const FelicaData* card2_data = nfc_device_get_data(card2, NfcProtocolFelica);
            const uint8_t card2_block_count = card2_data->blocks_total;

            if(card1_block_count != card2_block_count ||
               card1_data->workflow_type != card2_data->workflow_type) {
               checks->compare_type = NfcCompareChecksType_Shallow;
               break;
            }

            checks->total_blocks = card1_block_count;

            for(size_t i = 0; i < card1_block_count && i < FELICA_STANDARD_MAX_BLOCK_COUNT; i++) {
               const uint8_t* card1_block =
                  &card1_data->data.dump[i * (FELICA_DATA_BLOCK_SIZE + 2)];
               const uint8_t* card2_block =
                  &card2_data->data.dump[i * (FELICA_DATA_BLOCK_SIZE + 2)];

               if(memcmp(card1_block, card2_block, FELICA_DATA_BLOCK_SIZE) != 0) {
                  checks->diff_blocks[checks->diff_count] = i;
                  checks->diff_count++;
               }
            }
            break;
         }

         // NTAG/Mifare Ultralight
         case NfcProtocolMfUltralight: {
            const MfUltralightData* card1_data =
               nfc_device_get_data(card1, NfcProtocolMfUltralight);
            const uint16_t card1_block_count = mf_ultralight_get_pages_total(card1_data->type);
            const MfUltralightData* card2_data =
               nfc_device_get_data(card2, NfcProtocolMfUltralight);
            const uint16_t card2_block_count = mf_ultralight_get_pages_total(card2_data->type);

            if(card1_data->type != card2_data->type || card1_block_count != card2_block_count) {
               checks->compare_type = NfcCompareChecksType_Shallow;
               break;
            }

            checks->total_blocks = card1_block_count;

            for(size_t i = 0; i < card1_block_count && i < MF_ULTRALIGHT_MAX_PAGE_NUM; i++) {
               if(memcmp(
                     card1_data->page[i].data,
                     card2_data->page[i].data,
                     MF_ULTRALIGHT_PAGE_SIZE) != 0) {
                  checks->diff_blocks[checks->diff_count] = i;
                  checks->diff_count++;
               }
            }

            checks->diff_unit = NfcCompareChecksComparedDataType_Pages;

            break;
         }

         // Mifara Classic - @yoan8306
         case NfcProtocolMfClassic: {
            const MfClassicData* card1_data = nfc_device_get_data(card1, NfcProtocolMfClassic);
            const uint16_t card1_block_count = mf_classic_get_total_block_num(card1_data->type);
            const MfClassicData* card2_data = nfc_device_get_data(card2, NfcProtocolMfClassic);
            const uint16_t card2_block_count = mf_classic_get_total_block_num(card2_data->type);

            if(card1_data->type != card2_data->type || card1_block_count != card2_block_count) {
               checks->compare_type = NfcCompareChecksType_Shallow;
               break;
            }

            checks->total_blocks = card1_block_count;

            for(size_t i = 0; i < card1_block_count && i < MF_CLASSIC_TOTAL_BLOCKS_MAX; i++) {
               if(memcmp(
                     card1_data->block[i].data,
                     card2_data->block[i].data,
                     MF_CLASSIC_BLOCK_SIZE) != 0) {
                  checks->diff_blocks[checks->diff_count] = i;
                  checks->diff_count++;
               }
            }

            checks->diff_unit = NfcCompareChecksComparedDataType_Blocks;

            break;
         }

         // ST25TB
         case NfcProtocolSt25tb: {
            const St25tbData* card1_data = nfc_device_get_data(card1, NfcProtocolSt25tb);
            const uint8_t card1_block_count = st25tb_get_block_count(card1_data->type);
            const St25tbData* card2_data = nfc_device_get_data(card2, NfcProtocolSt25tb);
            const uint8_t card2_block_count = st25tb_get_block_count(card2_data->type);

            if(card1_data->type != card2_data->type || card1_block_count != card2_block_count) {
               checks->compare_type = NfcCompareChecksType_Shallow;
               break;
            }

            checks->total_blocks = card1_block_count;

            for(size_t i = 0; i < card1_block_count && i < ST25TB_MAX_BLOCKS; i++) {
               if(memcmp(&card1_data->blocks[i], &card2_data->blocks[i], ST25TB_BLOCK_SIZE) != 0) {
                  checks->diff_blocks[checks->diff_count] = i;
                  checks->diff_count++;
               }
            }

            checks->diff_unit = NfcCompareChecksComparedDataType_Blocks;

            break;
         }

         // Type 4 Tag
         case NfcProtocolType4Tag: {
            const Type4TagData* card1_data = nfc_device_get_data(card1, NfcProtocolType4Tag);
            const uint32_t card1_block_count = simple_array_get_count(card1_data->ndef_data);
            const Type4TagData* card2_data = nfc_device_get_data(card2, NfcProtocolType4Tag);
            const uint32_t card2_block_count = simple_array_get_count(card2_data->ndef_data);

            if(card1_block_count != card2_block_count ||
               card1_data->platform != card2_data->platform) {
               checks->compare_type = NfcCompareChecksType_Shallow;
               break;
            }

            checks->total_blocks = card1_block_count;

            const uint8_t* block1 = (const uint8_t*)simple_array_cget(card1_data->ndef_data, 0);
            const uint8_t* block2 = (const uint8_t*)simple_array_cget(card2_data->ndef_data, 0);

            for(size_t i = 0; i < card1_block_count && i < TYPE_4_TAG_MF_DESFIRE_NDEF_SIZE; i++) {
               if(block1[i] != block2[i]) {
                  checks->diff_blocks[checks->diff_count] = i;
                  checks->diff_count++;
               }
            }

            checks->diff_unit = NfcCompareChecksComparedDataType_Bytes;

            break;
         }

         // ISO15693-3
         case NfcProtocolIso15693_3: {
            const Iso15693_3Data* card1_data = nfc_device_get_data(card1, NfcProtocolIso15693_3);
            const uint8_t card1_block_size = card1_data->system_info.block_size;
            const uint16_t card1_block_count = card1_data->system_info.block_count;
            const Iso15693_3Data* card2_data = nfc_device_get_data(card2, NfcProtocolIso15693_3);
            const uint8_t card2_block_size = card2_data->system_info.block_size;
            const uint16_t card2_block_count = card2_data->system_info.block_count;

            if(card1_block_size != card2_block_size || card1_block_count != card2_block_count ||
               card1_data->system_info.ic_ref != card2_data->system_info.ic_ref) {
               checks->compare_type = NfcCompareChecksType_Shallow;
               break;
            }

            checks->total_blocks = card1_block_count;

            for(size_t i = 0; i < card1_block_count && i < 256; i++) {
               const uint8_t* block1 =
                  (const uint8_t*)simple_array_cget(card1_data->block_data, i * card1_block_size);
               const uint8_t* block2 =
                  (const uint8_t*)simple_array_cget(card2_data->block_data, i * card2_block_size);

               if(memcmp(block1, block2, card1_block_size) != 0) {
                  checks->diff_blocks[checks->diff_count] = i;
                  checks->diff_count++;
               }
            }

            checks->diff_unit = NfcCompareChecksComparedDataType_Blocks;

            break;
         }

         // All the others that are not supported or implemented yet
         default:
            break;
         }
      }
   }
}
