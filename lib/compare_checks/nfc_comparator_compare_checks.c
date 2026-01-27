#include "nfc_comparator_compare_checks.h"

NfcComparatorCompareChecks* nfc_comparator_compare_checks_alloc() {
   NfcComparatorCompareChecks* checks = malloc(sizeof(NfcComparatorCompareChecks));
   if(!checks) return NULL;

   checks->nfc_card_path = furi_string_alloc();
   checks->uid = false;
   checks->uid_length = false;
   checks->protocol = false;
   checks->nfc_data = false;
   checks->type = NfcCompareChecksType_Undefined;

   return checks;
}

void nfc_comparator_compare_checks_free(NfcComparatorCompareChecks* checks) {
   if(checks) {
      furi_string_free(checks->nfc_card_path);
      free(checks);
   }
}

void nfc_comparator_compare_checks_copy(
   NfcComparatorCompareChecks* destination,
   NfcComparatorCompareChecks* data) {
   furi_string_reset(destination->nfc_card_path);
   furi_string_set(destination->nfc_card_path, data->nfc_card_path);

   destination->uid = data->uid;
   destination->uid_length = data->uid_length;
   destination->protocol = data->protocol;
   destination->nfc_data = data->nfc_data;
   destination->type = data->type;
   destination->total_blocks = data->total_blocks;
   destination->diff_count = data->diff_count;
   memcpy(destination->diff_blocks, data->diff_blocks, sizeof(destination->diff_blocks));
}

void nfc_comparator_compare_checks_reset(NfcComparatorCompareChecks* checks) {
   if(checks) {
      furi_string_reset(checks->nfc_card_path);
      checks->uid = false;
      checks->uid_length = false;
      checks->protocol = false;
      checks->nfc_data = false;
      checks->type = NfcCompareChecksType_Undefined;
      checks->diff_count = 0;
      checks->total_blocks = 0;
   }
}

NfcCompareChecksType
   nfc_comparator_compare_checks_get_type(const NfcComparatorCompareChecks* checks) {
   furi_assert(checks);
   return checks->type;
}

void nfc_comparator_compare_checks_set_type(
   NfcComparatorCompareChecks* checks,
   NfcCompareChecksType type) {
   furi_assert(checks);
   checks->type = type;
}

void nfc_comparator_compare_checks_compare_cards(
   NfcComparatorCompareChecks* checks,
   const struct NfcDevice* card1,
   const struct NfcDevice* card2,
   bool check_data) {
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
   if(check_data) {
      checks->nfc_data = nfc_device_is_equal(card1, card2);

      // COMPARE LOGIC
      if(!checks->nfc_data && checks->protocol) {
         switch(nfc_device_get_protocol(card1)) {
         // Felica
         case NfcProtocolFelica: {
            break; // Not implemented
            const FelicaData* data1 = nfc_device_get_data(card1, NfcProtocolFelica);
            const FelicaData* data2 = nfc_device_get_data(card2, NfcProtocolFelica);

            uint8_t block_count = data1->blocks_total;
            checks->total_blocks = data1->blocks_total;

            for(uint8_t i = 0; i < block_count && i < 128; i++) {
               if(memcmp(&data1->data.dump[i], &data2->data.dump[i], sizeof(data1->data.dump[i])) !=
                  0) {
                  checks->diff_blocks[checks->diff_count] = i;
                  checks->diff_count++;
               }
            }
            break;
         }

         // Mifare Ultralight
         case NfcProtocolMfUltralight: {
            const MfUltralightData* data1 = nfc_device_get_data(card1, NfcProtocolMfUltralight);
            const MfUltralightData* data2 = nfc_device_get_data(card2, NfcProtocolMfUltralight);

            MfUltralightType type = data1->type;
            uint16_t block_count = mf_ultralight_get_pages_total(type);
            checks->total_blocks = block_count;

            for(uint16_t i = 0; i < block_count && i < 231; i++) {
               if(memcmp(data1->page[i].data, data2->page[i].data, sizeof(data1->page[i].data)) !=
                  0) {
                  checks->diff_blocks[checks->diff_count] = i;
                  checks->diff_count++;
               }
            }
            break;
         }

         // Mifara Classic - @yoan8306
         case NfcProtocolMfClassic: {
            const MfClassicData* data1 = nfc_device_get_data(card1, NfcProtocolMfClassic);
            const MfClassicData* data2 = nfc_device_get_data(card2, NfcProtocolMfClassic);

            MfClassicType type = data1->type;
            uint16_t block_count = mf_classic_get_total_block_num(type);

            checks->total_blocks = block_count;

            for(uint16_t i = 0; i < block_count && i < 64; i++) {
               if(memcmp(
                     data1->block[i].data, data2->block[i].data, sizeof(data1->block[i].data)) !=
                  0) {
                  checks->diff_blocks[checks->diff_count] = i;
                  checks->diff_count++;
               }
            }
            break;
         }

         // ST25TB
         case NfcProtocolSt25tb: {
            break; // Not implemented
            const St25tbData* data1 = nfc_device_get_data(card1, NfcProtocolSt25tb);
            const St25tbData* data2 = nfc_device_get_data(card2, NfcProtocolSt25tb);

            St25tbType type = data1->type;
            uint8_t block_count = st25tb_get_block_count(type);
            checks->total_blocks = block_count;

            for(uint8_t i = 0; i < block_count && i < 128; i++) {
               if(memcmp(&data1->blocks[i], &data2->blocks[i], sizeof(data1->blocks[i])) != 0) {
                  checks->diff_blocks[checks->diff_count] = i;
                  checks->diff_count++;
               }
            }
            break;
         }

         // Type 4 Tag
         case NfcProtocolType4Tag: {
            const Type4TagData* data1 = nfc_device_get_data(card1, NfcProtocolType4Tag);
            const Type4TagData* data2 = nfc_device_get_data(card2, NfcProtocolType4Tag);

            uint32_t block_count = simple_array_get_count(data1->ndef_data);
            checks->total_blocks = block_count;

            const uint8_t* block1 = (const uint8_t*)simple_array_cget(data1->ndef_data, 0);
            const uint8_t* block2 = (const uint8_t*)simple_array_cget(data2->ndef_data, 0);

            for(size_t i = 0; i < block_count && i < 2048; i++) {
               if(block1[i] != block2[i]) {
                  checks->diff_blocks[checks->diff_count] = i;
                  checks->diff_count++;
               }
            }
            break;
         }

         // All the others that are not supported or implemented yet
         default:
            break;
         }
      }
   }
}
