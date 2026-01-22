#include "../nfc_comparator.h"
#include <dolphin/dolphin.h>

static const char* match_str(bool match) {
   return match ? "Match" : "Mismatch";
}

void nfc_comparator_compare_results_scene_on_enter(void* context) {
   furi_assert(context);
   NfcComparator* nfc_comparator = context;

   nfc_comparator_led_worker_start(
      nfc_comparator->notification_app, NfcComparatorLedState_Complete);

   // Give points for performing a comparison
   dolphin_deed(DolphinDeedNfcRead);

   // Reset and build text
   furi_string_reset(nfc_comparator->views.text_box_store);

   switch(nfc_comparator->workers.compare_checks->type) {
   case NfcCompareChecksType_Physical:
      furi_string_printf(
         nfc_comparator->views.text_box_store,
         "UID: %s\nUID length: %s\nProtocol: %s",
         match_str(nfc_comparator->workers.compare_checks->uid),
         match_str(nfc_comparator->workers.compare_checks->uid_length),
         match_str(nfc_comparator->workers.compare_checks->protocol));
      break;
   case NfcCompareChecksType_Digital:
      furi_string_printf(
         nfc_comparator->views.text_box_store,
         "UID: %s\nUID length: %s\nProtocol: %s\nNFC Data: %s",
         match_str(nfc_comparator->workers.compare_checks->uid),
         match_str(nfc_comparator->workers.compare_checks->uid_length),
         match_str(nfc_comparator->workers.compare_checks->protocol),
         match_str(nfc_comparator->workers.compare_checks->nfc_data));

      // If NFC data is different, display all different blocks
      if(!nfc_comparator->workers.compare_checks->nfc_data && 
         nfc_comparator->workers.compare_checks->diff_count > 0) {
         
         // Calculate similarity percentage
         uint16_t total = nfc_comparator->workers.compare_checks->total_blocks;
         uint16_t diff = nfc_comparator->workers.compare_checks->diff_count;
         uint16_t similar = total - diff;
         uint8_t percentage = (similar * 100) / total;
         
         furi_string_cat_printf(nfc_comparator->views.text_box_store, 
            "\n\nSimilarity: %u%% (%u/%u blocks match)\n\nDifferent blocks (%u):\n", 
            percentage, similar, total, diff);
         
         // Display all blocks, 10 per line for better readability
         for(int i = 0; i < diff; i++) {
            furi_string_cat_printf(nfc_comparator->views.text_box_store, "%d", 
               nfc_comparator->workers.compare_checks->diff_blocks[i]);
            
            // Add comma except for the last one
            if(i < diff - 1) {
               furi_string_cat_printf(nfc_comparator->views.text_box_store, ", ");
               
               // Line break every 10 blocks
               if((i + 1) % 10 == 0) {
                  furi_string_cat_printf(nfc_comparator->views.text_box_store, "\n");
               }
            }
         }
      }
      break;
   default:
      furi_string_printf(nfc_comparator->views.text_box_store, "Unknown comparison type.");
      break;
   }

   // Configure TextBox with persistent text
   text_box_reset(nfc_comparator->views.text_box);
   text_box_set_text(nfc_comparator->views.text_box, furi_string_get_cstr(nfc_comparator->views.text_box_store));
   text_box_set_font(nfc_comparator->views.text_box, TextBoxFontText);

   view_dispatcher_switch_to_view(nfc_comparator->view_dispatcher, NfcComparatorView_TextBox);
}

bool nfc_comparator_compare_results_scene_on_event(void* context, SceneManagerEvent event) {
   NfcComparator* nfc_comparator = context;
   bool consumed = false;
   
   if(event.type == SceneManagerEventTypeBack) {
      scene_manager_previous_scene(nfc_comparator->scene_manager);
      consumed = true;
   }
   
   return consumed;
}

void nfc_comparator_compare_results_scene_on_exit(void* context) {
   furi_assert(context);
   NfcComparator* nfc_comparator = context;
   text_box_reset(nfc_comparator->views.text_box);
   furi_string_reset(nfc_comparator->views.text_box_store);
   nfc_comparator_led_worker_stop(nfc_comparator->notification_app);
   nfc_comparator_compare_checks_reset(nfc_comparator->workers.compare_checks);
}