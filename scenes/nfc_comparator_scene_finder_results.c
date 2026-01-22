#include "../nfc_comparator.h"

void nfc_comparator_finder_results_scene_on_enter(void* context) {
   furi_assert(context);
   NfcComparator* nfc_comparator = context;

   nfc_comparator_led_worker_start(
      nfc_comparator->notification_app, NfcComparatorLedState_Complete);

   // Reset and build text
   furi_string_reset(nfc_comparator->views.text_box.store);

   bool match = false;
   switch(nfc_comparator->workers.compare_checks->type) {
   case NfcCompareChecksType_Digital:
      match = nfc_comparator->workers.compare_checks->uid &&
              nfc_comparator->workers.compare_checks->uid_length &&
              nfc_comparator->workers.compare_checks->protocol &&
              nfc_comparator->workers.compare_checks->nfc_data;
      break;
   case NfcCompareChecksType_Physical:
      match = nfc_comparator->workers.compare_checks->uid &&
              nfc_comparator->workers.compare_checks->uid_length &&
              nfc_comparator->workers.compare_checks->protocol;
      break;
   default:
      furi_string_printf(nfc_comparator->views.text_box.store, "Unknown comparison type.");
      break;
   }

   if(match) {
      // Match found = more points!
      dolphin_deed(DolphinDeedNfcReadSuccess);

      furi_string_printf(
         nfc_comparator->views.text_box.store,
         "Match found!\n\n%s",
         furi_string_get_cstr(nfc_comparator->workers.compare_checks->nfc_card_path));
   } else {
      // No match but we made the effort
      dolphin_deed(DolphinDeedNfcRead);

      furi_string_printf(nfc_comparator->views.text_box.store, "No match found!");

      // Display all different blocks if present
      if(nfc_comparator->workers.compare_checks->diff_count > 0) {
         // Calculate similarity percentage
         uint16_t total = nfc_comparator->workers.compare_checks->total_blocks;
         uint16_t diff = nfc_comparator->workers.compare_checks->diff_count;
         uint16_t similar = total - diff;
         uint8_t percentage = (similar * 100) / total;

         furi_string_cat_printf(
            nfc_comparator->views.text_box.store,
            "\n\nSimilarity:\n%u%% (%u/%u blocks match)\n\nDifferent blocks (%u):\n",
            percentage,
            similar,
            total,
            diff);

         // Display all blocks, 10 per line
         for(int i = 0; i < diff; i++) {
            furi_string_cat_printf(
               nfc_comparator->views.text_box.store,
               "%d",
               nfc_comparator->workers.compare_checks->diff_blocks[i]);

            // Add comma except for the last one
            if(i < diff - 1) {
               furi_string_cat_printf(nfc_comparator->views.text_box.store, ", ");

               // Line break every 5 blocks
               if((i + 1) % 5 == 0) {
                  furi_string_cat_printf(nfc_comparator->views.text_box.store, "\n");
               }
            }
         }
      }
   }

   // Configure TextBox with persistent text
   text_box_reset(nfc_comparator->views.text_box.view);
   text_box_set_text(
      nfc_comparator->views.text_box.view,
      furi_string_get_cstr(nfc_comparator->views.text_box.store));
   text_box_set_font(nfc_comparator->views.text_box.view, TextBoxFontText);

   view_dispatcher_switch_to_view(nfc_comparator->view_dispatcher, NfcComparatorView_TextBox);
}

bool nfc_comparator_finder_results_scene_on_event(void* context, SceneManagerEvent event) {
   NfcComparator* nfc_comparator = context;
   bool consumed = false;

   if(event.type == SceneManagerEventTypeBack) {
      scene_manager_search_and_switch_to_previous_scene(
         nfc_comparator->scene_manager, NfcComparatorScene_FinderMenu);
      consumed = true;
   }

   return consumed;
}

void nfc_comparator_finder_results_scene_on_exit(void* context) {
   furi_assert(context);
   NfcComparator* nfc_comparator = context;
   text_box_reset(nfc_comparator->views.text_box.view);
   furi_string_reset(nfc_comparator->views.text_box.store);
   nfc_comparator_led_worker_stop(nfc_comparator->notification_app);
   nfc_comparator_compare_checks_reset(nfc_comparator->workers.compare_checks);
}
