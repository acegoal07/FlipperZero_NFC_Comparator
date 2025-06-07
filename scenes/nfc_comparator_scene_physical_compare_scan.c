#include "../nfc_comparator.h"

static volatile bool force_quit = false;

void nfc_comparator_physical_compare_scan_scene_on_enter(void* context) {
   furi_assert(context);
   NfcComparator* nfc_comparator = context;
   force_quit = false;

   popup_set_context(nfc_comparator->views.popup, nfc_comparator);
   view_dispatcher_switch_to_view(nfc_comparator->view_dispatcher, NfcComparatorView_Popup);

   nfc_comparator->workers.reader_worker =
      nfc_comparator_reader_worker_alloc(nfc_comparator->workers.compare_checks);

   if(!nfc_comparator_reader_worker_set_loaded_nfc_card(
         nfc_comparator->workers.reader_worker,
         furi_string_get_cstr(nfc_comparator->views.file_browser.output))) {
      scene_manager_next_scene(
         nfc_comparator->scene_manager, NfcComparatorScene_FailedToLoadNfcCard);
   } else {
      nfc_comparator_reader_worker_start(nfc_comparator->workers.reader_worker);
      nfc_comparator_led_worker_start(
         nfc_comparator->notification_app, NfcComparatorLedState_Running);
   }
}

bool nfc_comparator_physical_compare_scan_scene_on_event(void* context, SceneManagerEvent event) {
   furi_assert(context);
   NfcComparator* nfc_comparator = context;
   bool consumed = false;

   if(event.type == SceneManagerEventTypeBack) {
      force_quit = true;
      nfc_comparator_reader_worker_stop(nfc_comparator->workers.reader_worker);
      scene_manager_search_and_switch_to_previous_scene(
         nfc_comparator->scene_manager, NfcComparatorScene_CompareMenu);
      consumed = true;
   } else if(event.type == SceneManagerEventTypeTick) {
      switch(*nfc_comparator_reader_worker_get_state(nfc_comparator->workers.reader_worker)) {
      case NfcComparatorReaderWorkerState_Scanning:
         popup_set_header(
            nfc_comparator->views.popup, "Scanning....", 64, 5, AlignCenter, AlignTop);
         popup_set_text(
            nfc_comparator->views.popup,
            "Hold card next\nto Flipper's back",
            64,
            40,
            AlignCenter,
            AlignTop);
         break;
      case NfcComparatorReaderWorkerState_Polling:
         popup_set_header(
            nfc_comparator->views.popup, "Polling....", 64, 5, AlignCenter, AlignTop);
         break;
      case NfcComparatorReaderWorkerState_Comparing:
         popup_set_header(
            nfc_comparator->views.popup, "Comparing....", 64, 5, AlignCenter, AlignTop);
         break;
      case NfcComparatorReaderWorkerState_Stopped:
         if(!force_quit) {
            nfc_comparator_reader_worker_stop(nfc_comparator->workers.reader_worker);

            nfc_comparator_compare_checks_set_type(
               nfc_comparator->workers.compare_checks, NfcCompareChecksType_Physical);

            scene_manager_next_scene(
               nfc_comparator->scene_manager, NfcComparatorScene_CompareResults);
         }
         consumed = true;
         break;
      default: {
         break;
      }
      }
   }
   return consumed;
}

void nfc_comparator_physical_compare_scan_scene_on_exit(void* context) {
   furi_assert(context);
   NfcComparator* nfc_comparator = context;
   popup_reset(nfc_comparator->views.popup);
   if(nfc_comparator_compare_checks_get_type(nfc_comparator->workers.compare_checks) ==
      NfcCompareChecksType_Undefined) {
      nfc_comparator_compare_checks_reset(nfc_comparator->workers.compare_checks);
   }
   nfc_comparator_led_worker_stop(nfc_comparator->notification_app);
   nfc_comparator_reader_worker_free(nfc_comparator->workers.reader_worker);
}
